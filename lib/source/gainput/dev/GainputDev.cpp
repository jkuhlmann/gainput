
#include <gainput/gainput.h>

#ifdef GAINPUT_DEV
#include "GainputDev.h"
#include "GainputNetAddress.h"
#include "GainputNetListener.h"
#include "GainputNetConnection.h"
#include "GainputMemoryStream.h"
#include "../GainputInputDeltaState.h"
#include "../GainputHelpers.h"

#if _MSC_VER
#define snprintf _snprintf
#endif

namespace gainput
{

static NetListener* devListener = 0;
static NetConnection* devConnection = 0;
static InputManager* inputManager = 0;
static Allocator* allocator = 0;
static Array<const InputMap*> devMaps(GetDefaultAllocator());
static Array<const InputDevice*> devDevices(GetDefaultAllocator());
static bool devSendInfos = false;
static Array<DeviceId> devSyncedDevices(GetDefaultAllocator());

class DevUserButtonListener : public MappedInputListener
{
public:
	DevUserButtonListener(const InputMap* map) : map_(map) { }

	void OnUserButtonBool(UserButtonId userButton, bool oldValue, bool newValue)
	{
		if (!devConnection || !devSendInfos)
		{
			return;
		}

		Stream* stream = allocator->New<MemoryStream>(32, *allocator);
		stream->Write(uint8_t(DevCmdUserButtonChanged));
		stream->Write(uint32_t(map_->GetId()));
		stream->Write(uint32_t(userButton));
		stream->Write(uint8_t(0));
		stream->Write(uint8_t(newValue));
		stream->SeekBegin(0);
		devConnection->Send(*stream);
		allocator->Delete(stream);
	}

	void OnUserButtonFloat(UserButtonId userButton, float oldValue, float newValue)
	{
		if (!devConnection || !devSendInfos)
		{
			return;
		}

		Stream* stream = allocator->New<MemoryStream>(32, *allocator);
		stream->Write(uint8_t(DevCmdUserButtonChanged));
		stream->Write(uint32_t(map_->GetId()));
		stream->Write(uint32_t(userButton));
		stream->Write(uint8_t(1));
		stream->Write(newValue);
		stream->SeekBegin(0);
		devConnection->Send(*stream);
		allocator->Delete(stream);
	}

private:
	const InputMap* map_;
};

class DevDeviceButtonListener : public InputListener
{
public:
	DevDeviceButtonListener(const InputManager* inputManager) : inputManager_(inputManager) { }

	void OnDeviceButtonBool(DeviceId deviceId, DeviceButtonId deviceButton, bool oldValue, bool newValue)
	{
		if (!devConnection || devSyncedDevices.find(deviceId) == devSyncedDevices.end())
			return;
		const InputDevice* device = inputManager_->GetDevice(deviceId);
		GAINPUT_ASSERT(device);
		Stream* stream = allocator->New<MemoryStream>(32, *allocator);
		stream->Write(uint8_t(DevCmdSetDeviceButton));
		stream->Write(uint8_t(device->GetType()));
		stream->Write(uint8_t(device->GetIndex()));
		stream->Write(uint32_t(deviceButton));
		stream->Write(uint8_t(newValue));
		stream->SeekBegin(0);
		devConnection->Send(*stream);
		allocator->Delete(stream);
	}

	void OnDeviceButtonFloat(DeviceId deviceId, DeviceButtonId deviceButton, float oldValue, float newValue)
	{
		if (!devConnection || devSyncedDevices.find(deviceId) == devSyncedDevices.end())
			return;
		const InputDevice* device = inputManager_->GetDevice(deviceId);
		GAINPUT_ASSERT(device);
		Stream* stream = allocator->New<MemoryStream>(32, *allocator);
		stream->Write(uint8_t(DevCmdSetDeviceButton));
		stream->Write(uint8_t(device->GetType()));
		stream->Write(uint8_t(device->GetIndex()));
		stream->Write(uint32_t(deviceButton));
		stream->Write(newValue);
		stream->SeekBegin(0);
		devConnection->Send(*stream);
		allocator->Delete(stream);
	}

private:
	const InputManager* inputManager_;
};

static HashMap<const InputMap*, DevUserButtonListener*> devUserButtonListeners(GetDefaultAllocator());
DevDeviceButtonListener* devDeviceButtonListener = 0;
ListenerId devDeviceButtonListenerId;



void
SendDevice(const InputDevice* device, Stream* stream, NetConnection* devConnection)
{
	const DeviceId deviceId = device->GetDeviceId();
	stream->Reset();
	stream->Write(uint8_t(DevCmdDevice));
	stream->Write(uint32_t(device->GetDeviceId()));
	char deviceName[64];
	snprintf(deviceName, 64, "%s%d", device->GetTypeName(), device->GetIndex());
	stream->Write(uint8_t(strlen(deviceName)));
	stream->Write(deviceName, strlen(deviceName));
	stream->SeekBegin(0);
	devConnection->Send(*stream);

	for (DeviceButtonId buttonId = 0; buttonId < 1000; ++buttonId)
	{
		if (device->IsValidButtonId(buttonId))
		{
			stream->Reset();
			stream->Write(uint8_t(DevCmdDeviceButton));
			stream->Write(uint32_t(deviceId));
			stream->Write(uint32_t(buttonId));
			char buttonName[128];
			const size_t len= device->GetButtonName(buttonId, buttonName, 128);
			stream->Write(uint8_t(len));
			if (len)
				stream->Write(buttonName, len);
			stream->Write(uint8_t(device->GetButtonType(buttonId)));
			stream->SeekBegin(0);
			devConnection->Send(*stream);
		}
	}
}

void
SendMap(const InputMap* map, Stream* stream, NetConnection* devConnection)
{
	stream->Reset();
	stream->Write(uint8_t(DevCmdMap));
	stream->Write(uint32_t(map->GetId()));
	stream->Write(uint8_t(strlen(map->GetName())));
	stream->Write(map->GetName(), strlen(map->GetName()));
	stream->SeekBegin(0);
	devConnection->Send(*stream);

	DeviceButtonSpec mappings[32];
	for (UserButtonId buttonId = 0; buttonId < 1000; ++buttonId)
	{
		if (map->IsMapped(buttonId))
		{
			const size_t n = map->GetMappings(buttonId, mappings, 32);
			for (size_t i = 0; i < n; ++i)
			{
				stream->Reset();
				stream->Write(uint8_t(DevCmdUserButton));
				stream->Write(uint32_t(map->GetId()));
				stream->Write(uint32_t(buttonId));
				stream->Write(uint32_t(mappings[i].deviceId));
				stream->Write(uint32_t(mappings[i].buttonId));
				stream->Write(map->GetFloat(buttonId));
				stream->SeekBegin(0);
				devConnection->Send(*stream);
			}
		}
	}
}

void
DevInit(InputManager* manager)
{
	if (devListener)
	{
		return;
	}

#if defined(GAINPUT_PLATFORM_WIN)
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0)
	{
		GAINPUT_ASSERT(false);
	}
#endif

	inputManager = manager;
	allocator = &manager->GetAllocator();

	NetAddress address("0.0.0.0", 1211);
	devListener = allocator->New<NetListener>(address, *allocator);

	if (!devListener->Start(false))
	{
		GAINPUT_LOG("TOOL: Unable to listen\n");
	}

	GAINPUT_LOG("TOOL: Listening...\n");
}

void
DevShutdown(const InputManager* manager)
{
	if (inputManager != manager)
	{
		return;
	}

	GAINPUT_LOG("TOOL: Shutdown\n");

	if (devConnection)
	{
		devConnection->Close();
		allocator->Delete(devConnection);
		devConnection = 0;
	}

	if (devListener)
	{
		devListener->Stop();
		allocator->Delete(devListener);
		devListener = 0;
	}

	for (HashMap<const InputMap*, DevUserButtonListener*>::iterator it = devUserButtonListeners.begin();
			it != devUserButtonListeners.end();
			++it)
	{
		allocator->Delete(it->second);
	}

	if (devDeviceButtonListener)
	{
		inputManager->RemoveListener(devDeviceButtonListenerId);
		allocator->Delete(devDeviceButtonListener);
		devDeviceButtonListener = 0;
	}

#if defined(GAINPUT_PLATFORM_WIN)
	WSACleanup();
#endif
}


void
DevUpdate(InputDeltaState* delta)
{
	if (!devConnection)
	{
		devConnection = devListener->Accept();
		if (devConnection)
		{
			GAINPUT_LOG("TOOL: New connection\n");
			Stream* stream = allocator->New<MemoryStream>(1024, *allocator);

			stream->Write(uint8_t(DevCmdHello));
			stream->Write(uint32_t(DevProtocolVersion));
			stream->Write(uint32_t(GetLibVersion()));
			stream->SeekBegin(0);
			devConnection->Send(*stream);

			allocator->Delete(stream);
		}
	}

	if (!devConnection)
	{
		return;
	}

	Stream* stream = allocator->New<MemoryStream>(1024, *allocator);
	size_t received = 1;
	size_t r;
	while (received > 0)
	{
		stream->Reset();
		received = devConnection->Receive(*stream, 1024);
		if (!received)
			break;
		uint8_t cmd;
		r = stream->Read(cmd);
		GAINPUT_ASSERT(r);
		if (cmd == DevCmdGetAllInfos)
		{
			devSendInfos = true;

			int count = 0;
			// Send existing devices
			for (DeviceId deviceId = 0; deviceId < 1000; ++deviceId)
			{
				const InputDevice* device = inputManager->GetDevice(deviceId);
				if (!device)
				{
					break;
				}
				SendDevice(device, stream, devConnection);
				++count;
			}
			GAINPUT_LOG("TOOL: Sent %d devics.\n", count);

			// Send existing maps
			count = 0;
			for (Array<const InputMap*>::const_iterator it = devMaps.begin();
					it != devMaps.end();
					++it)
			{
				const InputMap* map = *it;
				SendMap(map, stream, devConnection);
				++count;
			}
			GAINPUT_LOG("TOOL: Sent %d maps.\n", count);
		}
		else if (cmd == DevCmdStartDeviceSync)
		{
			uint8_t deviceType;
			uint8_t deviceIndex;
			r = stream->Read(deviceType);
			r += stream->Read(deviceIndex);
			GAINPUT_ASSERT(r == sizeof(uint8_t)*2);
			const DeviceId deviceId = inputManager->FindDeviceId(InputDevice::DeviceType(deviceType), deviceIndex);
			InputDevice* device = inputManager->GetDevice(deviceId);
			GAINPUT_ASSERT(device);
			device->SetSynced(true);
			devSyncedDevices.push_back(deviceId);
			GAINPUT_LOG("TOOL: Starting to sync device #%d.\n", deviceId);
		}
		else if (cmd == DevCmdSetDeviceButton)
		{
			uint8_t deviceType;
			uint8_t deviceIndex;
			uint32_t deviceButtonId;
			r = stream->Read(deviceType);
			r += stream->Read(deviceIndex);
			r += stream->Read(deviceButtonId);
			GAINPUT_ASSERT(r == sizeof(uint8_t)*2 + sizeof(uint32_t));
			const DeviceId deviceId = inputManager->FindDeviceId(InputDevice::DeviceType(deviceType), deviceIndex);
			InputDevice* device = inputManager->GetDevice(deviceId);
			GAINPUT_ASSERT(device);
			GAINPUT_ASSERT(device->IsValidButtonId(deviceButtonId));
			GAINPUT_ASSERT(device->GetInputState());
			GAINPUT_ASSERT(device->GetPreviousInputState());
			if (device->GetButtonType(deviceButtonId) == BT_BOOL)
			{
				uint8_t value;
				r = stream->Read(value);
				GAINPUT_ASSERT(r == 1);
				bool boolValue = bool(value);
				HandleButton(deviceId, *device->GetInputState(), *device->GetPreviousInputState(), delta, deviceButtonId, boolValue);
			}
			else
			{
				float value;
				r = stream->Read(value);
				GAINPUT_ASSERT(r == sizeof(float));
				HandleAxis(deviceId, *device->GetInputState(), *device->GetPreviousInputState(), delta, deviceButtonId, value);
			}
		}
	}
	allocator->Delete(stream);

	const uint8_t cmd = DevCmdPing;
	if (!devConnection->IsConnected() || devConnection->Send(&cmd, sizeof(cmd)) == 0)
	{
		GAINPUT_LOG("TOOL: Disconnected\n");
		devConnection->Close();
		allocator->Delete(devConnection);
		devConnection = 0;

		for (Array<DeviceId>::iterator it = devSyncedDevices.begin();
				it != devSyncedDevices.end();
				++it)
		{
			InputDevice* device = inputManager->GetDevice(*it);
			GAINPUT_ASSERT(device);
			device->SetSynced(false);
			GAINPUT_LOG("TOOL: Stopped syncing device #%d.\n", *it);
		}
		devSyncedDevices.clear();

		if (devDeviceButtonListener)
		{
			inputManager->RemoveListener(devDeviceButtonListenerId);
			allocator->Delete(devDeviceButtonListener);
			devDeviceButtonListener = 0;
		}

		return;
	}
}

void
DevNewMap(InputMap* inputMap)
{
	if (devMaps.find(inputMap) != devMaps.end())
	{
		return;
	}
	devMaps.push_back(inputMap);

	DevUserButtonListener* listener = allocator->New<DevUserButtonListener>(inputMap);
	devUserButtonListeners[inputMap] = listener;
	inputMap->AddListener(listener);

	if (devConnection && devSendInfos)
	{
		Stream* stream = allocator->New<MemoryStream>(1024, *allocator);
		SendMap(inputMap, stream, devConnection);
		allocator->Delete(stream);
	}
}

void
DevNewUserButton(InputMap* inputMap, UserButtonId userButton, DeviceId device, DeviceButtonId deviceButton)
{
	if (!devConnection || devMaps.find(inputMap) == devMaps.end() || !devSendInfos)
	{
		return;
	}

	Stream* stream = allocator->New<MemoryStream>(1024, *allocator);
	stream->Write(uint8_t(DevCmdUserButton));
	stream->Write(uint32_t(inputMap->GetId()));
	stream->Write(uint32_t(userButton));
	stream->Write(uint32_t(device));
	stream->Write(uint32_t(deviceButton));
	stream->Write(inputMap->GetFloat(userButton));
	stream->SeekBegin(0);
	devConnection->Send(*stream);
	allocator->Delete(stream);
}

void
DevRemoveUserButton(InputMap* inputMap, UserButtonId userButton)
{
	if (!devConnection || !devSendInfos)
	{
		return;
	}

	Stream* stream = allocator->New<MemoryStream>(1024, *allocator);
	stream->Write(uint8_t(DevCmdRemoveUserButton));
	stream->Write(uint32_t(inputMap->GetId()));
	stream->Write(uint32_t(userButton));
	stream->SeekBegin(0);
	devConnection->Send(*stream);
	allocator->Delete(stream);
}

void
DevRemoveMap(InputMap* inputMap)
{
	if (!devConnection || devMaps.find(inputMap) == devMaps.end())
	{
		return;
	}

	if (devSendInfos)
	{
		Stream* stream = allocator->New<MemoryStream>(1024, *allocator);
		stream->Write(uint8_t(DevCmdRemoveMap));
		stream->Write(uint32_t(inputMap->GetId()));
		stream->SeekBegin(0);
		devConnection->Send(*stream);
		allocator->Delete(stream);
	}

	devMaps.erase(devMaps.find(inputMap));

	HashMap<const InputMap*, DevUserButtonListener*>::iterator it = devUserButtonListeners.find(inputMap);
	if (it != devUserButtonListeners.end())
	{
		allocator->Delete(it->second);
		devUserButtonListeners.erase(it->first);
	}
}

void
DevNewDevice(InputDevice* device)
{
	if (devDevices.find(device) != devDevices.end())
	{
		return;
	}
	devDevices.push_back(device);

	if (devConnection && devSendInfos)
	{
		Stream* stream = allocator->New<MemoryStream>(1024, *allocator);
		SendDevice(device, stream, devConnection);
		allocator->Delete(stream);
	}
}

void
DevConnect(InputManager* manager, const char* ip, unsigned port)
{
	if (devConnection)
	{
		devConnection->Close();
		allocator->Delete(devConnection);
	}

	if (devListener)
	{
		devListener->Stop();
		allocator->Delete(devListener);
		devListener = 0;
	}

	inputManager = manager;
	allocator = &manager->GetAllocator();

	devConnection = allocator->New<NetConnection>(NetAddress(ip, port));

	if (!devConnection->Connect(false) || !devConnection->IsConnected())
	{
		devConnection->Close();
		allocator->Delete(devConnection);
		devConnection = 0;
		GAINPUT_LOG("TOOL: Unable to connect to %s:%d.\n", ip, port);
	}

	GAINPUT_LOG("TOOL: Connected to %s:%d.\n", ip, port);

	if (!devDeviceButtonListener)
	{
		devDeviceButtonListener = allocator->New<DevDeviceButtonListener>(manager);
		devDeviceButtonListenerId = inputManager->AddListener(devDeviceButtonListener);
	}
}

void
DevStartDeviceSync(DeviceId deviceId)
{
	if (devSyncedDevices.find(deviceId) != devSyncedDevices.end())
		return;
	devSyncedDevices.push_back(deviceId);

	const InputDevice* device = inputManager->GetDevice(deviceId);
	GAINPUT_ASSERT(device);

	Stream* stream = allocator->New<MemoryStream>(32, *allocator);
	stream->Write(uint8_t(DevCmdStartDeviceSync));
	stream->Write(uint8_t(device->GetType()));
	stream->Write(uint8_t(device->GetIndex()));
	stream->SeekBegin(0);
	devConnection->Send(*stream);

	// Send device state
	for (DeviceButtonId buttonId = 0; buttonId < 1000; ++buttonId)
	{
		if (device->IsValidButtonId(buttonId))
		{
			stream->Reset();
			stream->Write(uint8_t(DevCmdSetDeviceButton));
			stream->Write(uint8_t(device->GetType()));
			stream->Write(uint8_t(device->GetIndex()));
			stream->Write(uint32_t(buttonId));
			if (device->GetButtonType(buttonId) == BT_BOOL)
			{
				stream->Write(uint8_t(device->GetBool(buttonId)));
			}
			else
			{
				stream->Write(device->GetFloat(buttonId));
			}
			stream->SeekBegin(0);
			devConnection->Send(*stream);
		}
	}

	allocator->Delete(stream);
}

}

#endif

