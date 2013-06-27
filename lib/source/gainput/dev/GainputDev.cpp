
#include <gainput/gainput.h>

#ifdef GAINPUT_DEV
#include "GainputDev.h"
#include "GainputNetListener.h"
#include "GainputNetConnection.h"
#include "GainputMemoryStream.h"

namespace gainput
{

static NetListener* devListener = 0;
static NetConnection* devConnection = 0;
static const InputManager* inputManager = 0;
static Allocator* allocator = 0;
static Array<const InputMap*> devMaps(GetDefaultAllocator());


class DevUserButtonListener : public MappedInputListener
{
public:
	DevUserButtonListener(const InputMap* map) : map_(map) { }

	void OnUserButtonBool(UserButtonId userButton, bool oldValue, bool newValue)
	{
		if (!devConnection)
		{
			return;
		}

		Stream* stream = allocator->New<MemoryStream>(32);
		stream->Write(uint8_t(DevCmdUserButtonChanged));
		stream->Write(uint64_t(map_));
		stream->Write(uint32_t(userButton));
		stream->Write(uint8_t(0));
		stream->Write(uint8_t(newValue));
		stream->SeekBegin(0);
		devConnection->Send(*stream);
		allocator->Delete(stream);
	}

	void OnUserButtonFloat(UserButtonId userButton, float oldValue, float newValue)
	{
		if (!devConnection)
		{
			return;
		}

		Stream* stream = allocator->New<MemoryStream>(32);
		stream->Write(uint8_t(DevCmdUserButtonChanged));
		stream->Write(uint64_t(map_));
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


void
DevInit(const InputManager* manager)
{
	if (devListener)
	{
		return;
	}

	inputManager = manager;
	allocator = &manager->GetAllocator();

	NetAddress address("0.0.0.0", 1211);
	devListener = allocator->New<NetListener>(address);

	if (!devListener->Start(false))
	{
		GAINPUT_LOG("TOOL: Unable to listen\n");
	}

	GAINPUT_LOG("TOOL: Listening...\n");
}

void
DevUpdate()
{
	if (!devConnection)
	{
		devConnection = devListener->Accept();
		if (devConnection)
		{
			GAINPUT_LOG("TOOL: New connection\n");
			char buf[1024];
			devConnection->Receive(buf, 1024);
			GAINPUT_LOG("TOOL: Got: %s\n", buf);
			int count = 0;
			Stream* stream = allocator->New<MemoryStream>(1024);

			for (DeviceId deviceId = 0; deviceId < 1000; ++deviceId)
			{
				const InputDevice* device = inputManager->GetDevice(deviceId);
				if (!device)
				{
					break;
				}

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
						uint32_t t = deviceId;
						stream->Write(t);
						t = buttonId;
						stream->Write(t);
						char buttonName[128];
						const size_t len= device->GetButtonName(buttonId, buttonName, 128);
						stream->Write(uint8_t(len));
						if (len)
							stream->Write(buttonName, len);
						stream->Write(uint8_t(device->GetButtonType(buttonId)));
						stream->SeekBegin(0);
						devConnection->Send(*stream);
						++count;
					}
				}
			}
			GAINPUT_LOG("TOOL: Sent %d device buttons.\n", count);

			count = 0;
			for (Array<const InputMap*>::const_iterator it = devMaps.begin();
					it != devMaps.end();
					++it)
			{
				const InputMap* map = *it;
				stream->Reset();
				stream->Write(uint8_t(DevCmdMap));
				stream->Write(uint64_t(map));
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
							stream->Write(uint64_t(map));
							stream->Write(uint32_t(buttonId));
							stream->Write(uint32_t(mappings[i].deviceId));
							stream->Write(uint32_t(mappings[i].buttonId));
							stream->SeekBegin(0);
							devConnection->Send(*stream);
							++count;
						}
					}
				}
			}
			GAINPUT_LOG("TOOL: Sent %d user buttons.\n", count);

			allocator->Delete(stream);
		}
	}

	if (!devConnection)
	{
		return;
	}

	const uint8_t cmd = DevCmdPing;
	if (!devConnection->IsConnected() || devConnection->Send(&cmd, sizeof(cmd)) == 0)
	{
		GAINPUT_LOG("TOOL: Disconnected\n");
		devConnection->Close();
		delete devConnection;
		devConnection = 0;
		return;
	}
}

void
DevNewMap(InputMap* inputMap)
{
	devMaps.push_back(inputMap);
	inputMap->AddListener(allocator->New<DevUserButtonListener>(inputMap));
	// TODO inform existing devConnection
}

}

#endif

