#include <gainput/gainput.h>

#ifdef GAINPUT_DEV
#include "GainputNetAddress.h"
#include "GainputNetConnection.h"

#include "GainputStream.h"

#if defined(GAINPUT_PLATFORM_LINUX)
#include <cassert>
#include <fcntl.h>
#include <errno.h>

namespace gainput {

NetConnection::NetConnection(const NetAddress& address) :
	address(address),
	fd(-1)
{
}

NetConnection::NetConnection(const NetAddress& remoteAddress, int fd) :
	address(remoteAddress),
	fd(fd)
{

}

NetConnection::~NetConnection()
{
	Close();
}

bool
NetConnection::Connect(bool shouldBlock)
{
	assert(fd == -1);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		return false;
	}

	if (connect(fd, (struct sockaddr*)&address.GetAddr(), sizeof(struct sockaddr_in)) == -1)
	{
		return false;
	}

	if (!shouldBlock && fcntl(fd, F_SETFL, O_NONBLOCK) == -1)
	{
		return false;
	}

	return true;
}

bool
NetConnection::IsConnected() const
{
	return fd != -1;
}

void
NetConnection::Close()
{
	if (fd == -1)
	{
		return;
	}

	shutdown(fd, SHUT_RDWR);
	fd = -1;
}

bool
NetConnection::IsReady(bool read, bool write)
{
	fd_set read_fds;
	fd_set write_fds;
	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);

	if (read)
	{
		FD_SET(fd, &read_fds);
	}
	if (write)
	{
		FD_SET(fd, &write_fds);
	}

	struct timeval timeout;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;

	int rc = select(sizeof(read_fds)*8, &read_fds, &write_fds, 0, &timeout);
	if (rc == -1)
	{
		return false;
	}

	bool result = false;
	if (rc > 0)
	{
		result = true;
		if (read && !FD_ISSET(fd, &read_fds))
		{
			result = false;
		}
		if (write && !FD_ISSET(fd, &write_fds))
		{
			result = false;
		}
	}

	return result;
}

size_t
NetConnection::Send(const void* buf, size_t length)
{
	assert(IsConnected());
	const int result = send(fd, buf, length, MSG_NOSIGNAL);
	if (result == -1 && errno == EPIPE)
	{
		Close();
	}
	return result >= 0 ? result : 0;
}

size_t
NetConnection::Send(Stream& stream)
{
	const size_t length = stream.GetLeft();
	char* buf = new char[length];
	stream.Read(buf, length);
	size_t sentLength = Send(buf, length);
	delete[] buf;
	return sentLength;
}

size_t
NetConnection::Receive(void* buffer, size_t length)
{
	assert(IsConnected());
	ssize_t receivedLength = recv(fd, buffer, length, 0);
	if (receivedLength == -1)
	{
		return 0;
	}
	return receivedLength;
}

size_t
NetConnection::Receive(Stream& stream, size_t maxLength)
{
	char* buf = new char[maxLength];
	size_t receivedLength = Receive(buf, maxLength);
	stream.Write(buf, receivedLength);
	stream.SeekBegin(0);
	delete[] buf;
	return receivedLength;
}

}

#endif
#endif

