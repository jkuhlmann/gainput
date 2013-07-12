
#include "Catch/single_include/catch.hpp"

#include <gainput/gainput.h>

using namespace gainput;

TEST_CASE("inputmap/init", "")
{
	InputManager manager;

	InputMap map(manager);

	REQUIRE(!map.IsMapped(0));
}

