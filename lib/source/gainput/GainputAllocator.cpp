
#include <gainput/gainput.h>

namespace gainput
{
DefaultAllocator& GetDefaultAllocator()
{
	static DefaultAllocator da;
	return da;
}
}

