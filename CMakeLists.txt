cmake_minimum_required(VERSION 2.8)

if(!WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

add_subdirectory(lib)
add_subdirectory(samples)
add_subdirectory(test)


