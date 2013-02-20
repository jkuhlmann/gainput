
#ifndef GAINPUTALLOCATOR_H_
#define GAINPUTALLOCATOR_H_

namespace gainput
{

/// Interface used to pass custom allocators to the library.
class GAINPUT_LIBEXPORT Allocator
{
public:
	enum { DefaultAlign = 0 };

	/// Allocates a number of bytes and returns a pointer to the allocated memory.
	/**
	 * \param size The number of bytes to allocate.
	 * \return A memory block encompassing at least size bytes.
	 */
	virtual void* Allocate(size_t size, size_t align = DefaultAlign) = 0;
	/// Deallocates the given memory.
	/**
	 * \param ptr The memory block to deallocate.
	 */
	virtual void Deallocate(void* ptr) = 0;

	/// An operator new-like function that allocates memory and calls T's constructor.
	/**
	 * \return A pointer to an initialized instance of T.
	 */
	template <class T>
	T* New()
	{
		return new (Allocate(sizeof(T))) T();
	}

	/// An operator new-like function that allocates memory and calls T's constructor with one parameter.
	/**
	 * \return A pointer to an initialized instance of T.
	 */
	template <class T, class P0>
	T* New(P0& p0)
	{
		return new (Allocate(sizeof(T))) T(p0);
	}

	/// An operator new-like function that allocates memory and calls T's constructor with two parameters.
	/**
	 * \return A pointer to an initialized instance of T.
	 */
	template <class T, class P0, class P1>
	T* New(P0& p0, P1& p1)
	{
		return new (Allocate(sizeof(T))) T(p0, p1);
	}

	/// An operator delete-like function that calls ptr's constructor and deallocates the memory.
	/**
	 * \param ptr The object to destruct and deallocate.
	 */
	template <class T>
	void Delete(T* ptr)
	{
		if (ptr)
		{
			ptr->~T();
			Deallocate(ptr);
		}
	}
};

/// The default allocator used by the library.
/**
 * Uses malloc and free.
 */
class GAINPUT_LIBEXPORT DefaultAllocator : public Allocator
{
public:
	void* Allocate(size_t size, size_t align = DefaultAlign)
	{
		return malloc(size);
	}

	void Deallocate(void* ptr)
	{
		free(ptr);
	}
};


GAINPUT_LIBEXPORT DefaultAllocator& GetDefaultAllocator();

}

#endif

