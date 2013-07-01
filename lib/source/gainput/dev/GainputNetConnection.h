#ifndef GAINPUTCONNECTION_H_
#define GAINPUTCONNECTION_H_

#include "GainputNetAddress.h"

namespace gainput {

class Stream;

class NetConnection
{
public:
	NetConnection(const NetAddress& address);
#if defined(GAINPUT_PLATFORM_LINUX) || defined(GAINPUT_PLATFORM_ANDROID)
	NetConnection(const NetAddress& remoteAddress, int fd);
#endif
	~NetConnection();

	bool Connect(bool shouldBlock);
	void Close();
	bool IsConnected() const;
	bool IsReady(bool read, bool write);

	size_t Send(const void* buffer, size_t length);
	size_t Send(Stream& stream);

	size_t Receive(void* buffer, size_t length);
	size_t Receive(Stream& stream, size_t maxLength);

private:
	NetAddress address;

#if defined(GAINPUT_PLATFORM_LINUX) || defined(GAINPUT_PLATFORM_ANDROID)
	int fd;
#endif

};

}

#endif

