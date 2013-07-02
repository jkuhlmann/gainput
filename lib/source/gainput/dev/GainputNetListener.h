#ifndef GAINPUTLISTENER_H_
#define GAINPUTLISTENER_H_

#include "GainputNetAddress.h"

namespace gainput {

class NetConnection;

class NetListener
{
public:
	NetListener(const NetAddress& address);
	~NetListener();

	bool Start(bool shouldBlock);
	void Stop();

	NetConnection* Accept();

private:
	NetAddress address;

#if defined(GAINPUT_PLATFORM_LINUX) || defined(GAINPUT_PLATFORM_ANDROID)
	int fd;
#elif defined(GAINPUT_PLATFORM_WIN)
	SOCKET listenSocket;
#endif

};

}

#endif

