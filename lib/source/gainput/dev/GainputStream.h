#ifndef GAINPUTSTREAM_H_
#define GAINPUTSTREAM_H_

namespace gainput {

class Stream
{
public:
	virtual size_t Read(void* dest, size_t length) = 0;
	virtual size_t Write(const void* src, size_t length) = 0;

	virtual size_t GetSize() const = 0;
	virtual size_t GetLeft() const = 0;

	virtual bool SeekBegin(int offset) = 0;
	virtual bool SeekCurrent(int offset) = 0;

	virtual void Reset() = 0;

	virtual bool IsEof() const = 0;

	template<class T> size_t Read(T& dest) { return Read(&dest, sizeof(T)); }
	template<class T> size_t Write(const T& src) { return Write(&src, sizeof(T)); }

};

}

#endif

