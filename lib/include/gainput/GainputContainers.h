
#ifndef GAINPUTCONTAINERS_H_
#define GAINPUTCONTAINERS_H_

#include <map>

namespace gainput
{


// MurmurHash3
// http://code.google.com/p/smhasher/wiki/MurmurHash3
// MurmurHash3 was written by Austin Appleby, and is placed in the public
// domain. The author hereby disclaims copyright to this source code.

inline uint32_t rotl32 ( uint32_t x, int8_t r )
{
  return (x << r) | (x >> (32 - r));
}

#define ROTL32(x,y)     rotl32(x,y)


inline uint32_t getblock ( const uint32_t * p, int i )
{
  return p[i];
}


inline uint32_t fmix ( uint32_t h )
{
  h ^= h >> 16;
  h *= 0x85ebca6b;
  h ^= h >> 13;
  h *= 0xc2b2ae35;
  h ^= h >> 16;

  return h;
}


inline void MurmurHash3_x86_32 ( const void * key, int len,
                          uint32_t seed, void * out )
{
  const uint8_t * data = (const uint8_t*)key;
  const int nblocks = len / 4;

  uint32_t h1 = seed;

  const uint32_t c1 = 0xcc9e2d51;
  const uint32_t c2 = 0x1b873593;

  //----------
  // body

  const uint32_t * blocks = (const uint32_t *)(data + nblocks*4);

  for(int i = -nblocks; i; i++)
  {
    uint32_t k1 = getblock(blocks,i);

    k1 *= c1;
    k1 = ROTL32(k1,15);
    k1 *= c2;
    
    h1 ^= k1;
    h1 = ROTL32(h1,13); 
    h1 = h1*5+0xe6546b64;
  }

  //----------
  // tail

  const uint8_t * tail = (const uint8_t*)(data + nblocks*4);

  uint32_t k1 = 0;

  switch(len & 3)
  {
  case 3: k1 ^= tail[2] << 16;
  case 2: k1 ^= tail[1] << 8;
  case 1: k1 ^= tail[0];
          k1 *= c1; k1 = ROTL32(k1,15); k1 *= c2; h1 ^= k1;
  };

  //----------
  // finalization

  h1 ^= len;

  h1 = fmix(h1);

  *(uint32_t*)out = h1;
}

// MurmurHash3 end


template<class T>
class Array
{
public:
	enum { DefaultCapacity = 8 };

	typedef T* iterator;
	typedef const T* const_iterator;
	typedef T value_type;

	Array(Allocator& allocator, unsigned capacity = DefaultCapacity) :
		allocator_(allocator),
		size_(0),
		capacity_(capacity)
	{
		data_ = static_cast<T*>(allocator_.Allocate(sizeof(T)*capacity_));
	}

	~Array()
	{
		allocator_.Delete(data_);
	}

	iterator begin() { return data_; }
	const_iterator begin() const { return data_; }
	iterator end() { return data_ + size_; }
	const_iterator end() const { return data_ + size_; }

	void push_back(const value_type& val)
	{
		if (size_ + 1 > capacity_)
		{
			reserve(size_ + 1);
		}
		data_[size_++] = val;
	}

	void reserve(unsigned capacity)
	{
		if (capacity <= capacity_)
			return;
		capacity = (capacity_*capacity_) < capacity ? capacity : (capacity_*capacity_);
		T* newData = static_cast<T*>(allocator_.Allocate(sizeof(T)*capacity));
		memcpy(newData, data_, sizeof(T)*capacity_);
		allocator_.Deallocate(data_);
		data_ = newData;
		capacity_ = capacity;
	}

	void clear() { size_ = 0; }

	bool empty() const { return size_ == 0; }

private:
	Allocator& allocator_;
	unsigned size_;
	unsigned capacity_;
	T* data_;
};

template<int N, class T>
class RingBuffer
{
public:
	RingBuffer() :
		nextRead_(0),
		nextWrite_(0)
	{ }


	bool CanGet() const
	{
		 return nextRead_ < nextWrite_;
	}

	unsigned GetCount() const
	{
		return nextWrite_ - nextRead_;
	}

	T Get()
	{
		return buf_[(nextRead_++) % N];
	}

	void Put(T d)
	{
		buf_[(nextWrite_++) % N] = d;
	}

private:
	T buf_[N];
	unsigned nextRead_;
	unsigned nextWrite_;
};


template<class K>
class DialectTable : public std::map<K, DeviceButtonId>
{

};

}

#endif

