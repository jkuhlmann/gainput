#ifndef GAINPUTDEVPROTOCOL_H_
#define GAINPUTDEVPROTOCOL_H_

namespace gainput
{

enum DevCmd
{
	DevCmdHello,
	DevCmdDevice,
	DevCmdDeviceButton,
	DevCmdMap,
	DevCmdRemoveMap,
	DevCmdUserButton,
	DevCmdPing,
	DevCmdUserButtonChanged,
};

const static unsigned DevProtocolVersion = 0x1;

}

#endif

