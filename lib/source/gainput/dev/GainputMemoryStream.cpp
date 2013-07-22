#include <gainput/gainput.h>

#ifdef GAINPUT_DEV
#include "GainputMemoryStream.h"

namespace gainput {

MemoryStream::MemoryStream(void* data, size_t length, size_t capacity, bool ownership) :
	data(data),
	length(length),
	capacity(capacity),
	ownership(ownership),
	position(0)
{
	// empty
}

MemoryStream::MemoryStream(size_t capacity, Allocator& allocator) :
	allocator(&allocator),
	length(0),
	capacity(capacity),
	ownership(true),
	position(0)
{
	data = this->allocator->Allocate(capacity);
}

MemoryStream::~MemoryStream()
{
	if (ownership)
	{
		assert(allocator);
		allocator->Deallocate(data);
	}
}

size_t
MemoryStream::Read(void* dest, size_t readLength)
{
	assert(position + readLength <= length);
	memcpy(dest, (void*)( (uint8_t*)data + position), readLength);
	position += readLength;
	return readLength;
}

size_t
MemoryStream::Write(const void* src, size_t writeLength)
{
	assert(position + writeLength <= capacity);
	memcpy((void*)( (uint8_t*)data + position), src, writeLength);
	position += writeLength;
	length += writeLength;
	return writeLength;
}

bool
MemoryStream::SeekBegin(int offset)
{
	if (offset < 0)
	{
		return false;
	}
	position = offset;
	return true;
}

bool
MemoryStream::SeekCurrent(int offset)
{
	if (offset + position > length)
	{
		return false;
	}
	position += offset;
	return true;
}

}
#endif

