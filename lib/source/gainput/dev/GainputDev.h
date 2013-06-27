
#ifndef GAINPUTDEV_H_
#define GAINPUTDEV_H_

#ifdef GAINPUT_DEV

#include "GainputDevProtocol.h"

namespace gainput
{

void DevInit(const InputManager* inputManager);
void DevUpdate();
void DevNewMap(InputMap* inputMap);

}

#define GAINPUT_DEV_INIT(inputManager)	DevInit(inputManager)
#define GAINPUT_DEV_UPDATE()		DevUpdate()
#define GAINPUT_DEV_NEW_MAP(inputMap)	DevNewMap(inputMap)

#else

#define GAINPUT_DEV_INIT(inputManager)
#define GAINPUT_DEV_UPDATE()
#define GAINPUT_DEV_NEW_MAP(inputMap)

#endif

#endif

