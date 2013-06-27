#include <gainput/gainput.h>

#ifdef GAINPUT_DEV
#include "GainputNetAddress.h"
#include "GainputNetConnection.h"
#include "GainputNetListener.h"

#if defined(GAINPUT_PLATFORM_LINUX)
#include <fcntl.h>

namespace gainput {

NetListener::NetListener(const NetAddress& address) :
	address(address),
	fd(-1)
{

}

NetListener::~NetListener()
{
	Stop();
}

bool
NetListener::Start(bool shouldBlock)
{
	assert(fd == -1);
	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		return false;
	}

	if (!shouldBlock && fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		return false;
	}

	if (bind(fd, (struct sockaddr*)&address.GetAddr(), sizeof(struct sockaddr_in)) == -1)
	{
		return false;
	}

	if (listen(fd, 50) == -1)
	{
		return false;
	}

	return true;
}

void
NetListener::Stop()
{
	if (fd == -1)
	{
		return;
	}

	shutdown(fd, SHUT_RDWR);
	fd = -1;
}

NetConnection*
NetListener::Accept()
{
	assert(fd != -1);
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(struct sockaddr_in);
	int remoteFd = accept(fd, (struct sockaddr*)&addr, &addr_len);
	if (remoteFd == -1)
	{
		return 0;
	}
	NetAddress remoteAddress(addr);
	NetConnection* connection = new NetConnection(remoteAddress, remoteFd);
	return connection;
}

}

#endif
#endif

