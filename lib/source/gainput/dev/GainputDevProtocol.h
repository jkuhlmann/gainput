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
	DevCmdUserButton,
	DevCmdPing,
	DevCmdUserButtonChanged,
};

}

#endif

