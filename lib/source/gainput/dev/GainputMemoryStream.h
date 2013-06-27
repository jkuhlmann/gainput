#ifndef GAINPUTMEMORYSTREAM_H_
#define GAINPUTMEMORYSTREAM_H_

#include "GainputStream.h"

namespace gainput {

class MemoryStream : public Stream
{
public:
	MemoryStream(void* data, size_t length, bool ownership = false);
	MemoryStream(size_t capacity);
	~MemoryStream();

	size_t Read(void* dest, size_t readLength);
	size_t Write(const void* src, size_t writeLength);

	size_t GetSize() const { return length; }
	size_t GetLeft() const { return length - position; }

	bool SeekBegin(int offset);
	bool SeekCurrent(int offset);

	virtual void Reset() { length = 0; position = 0; }

	bool IsEof() const
	{
		return position >= length;
	}

	void* GetData() { return data; }
	size_t GetPosition() const { return position; }

private:
	void* data;
	size_t length;
	size_t capacity;
	bool ownership;

	size_t position;

};

}

#endif

