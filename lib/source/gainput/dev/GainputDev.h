
#ifndef GAINPUTDEV_H_
#define GAINPUTDEV_H_

#ifdef GAINPUT_DEV

#include "GainputDevProtocol.h"

namespace gainput
{

void DevInit(const InputManager* inputManager);
void DevShutdown(const InputManager* inputManager);
void DevUpdate();
void DevNewMap(InputMap* inputMap);
void DevNewUserButton(InputMap* inputMap, UserButtonId userButton, DeviceId device, DeviceButtonId deviceButton);
void DevRemoveUserButton(InputMap* inputMap, UserButtonId userButton);
void DevRemoveMap(InputMap* inputMap);
void DevNewDevice(InputDevice* device);

}

#define GAINPUT_DEV_INIT(inputManager)	DevInit(inputManager)
#define GAINPUT_DEV_SHUTDOWN(inputManager)	DevShutdown(inputManager)
#define GAINPUT_DEV_UPDATE()		DevUpdate()
#define GAINPUT_DEV_NEW_MAP(inputMap)	DevNewMap(inputMap)
#define GAINPUT_DEV_NEW_USER_BUTTON(inputMap, userButton, device, deviceButton)	DevNewUserButton(inputMap, userButton, device, deviceButton)
#define GAINPUT_DEV_REMOVE_USER_BUTTON(inputMap, userButton)	DevRemoveUserButton(inputMap, userButton)
#define GAINPUT_DEV_REMOVE_MAP(inputMap)	DevRemoveMap(inputMap)
#define GAINPUT_DEV_NEW_DEVICE(device)	DevNewDevice(device)

#else

#define GAINPUT_DEV_INIT(inputManager)
#define GAINPUT_DEV_SHUTDOWN(inputManager)
#define GAINPUT_DEV_UPDATE()
#define GAINPUT_DEV_NEW_MAP(inputMap)
#define GAINPUT_DEV_NEW_USER_BUTTON(inputMap, userButton, device, deviceButton)
#define GAINPUT_DEV_REMOVE_USER_BUTTON(inputMap, userButton)
#define GAINPUT_DEV_REMOVE_MAP(inputMap)
#define GAINPUT_DEV_NEW_DEVICE(device)

#endif

#endif

