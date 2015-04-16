
project(gainputtest)

include_directories(../lib/include/)
include_directories(../extern/catch/)

file(GLOB_RECURSE sources *.cpp)

add_executable(gainputtest ${sources})

target_link_libraries(gainputtest gainputstatic)

find_package(X11)
find_package(OpenGL)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(gainputtest X11 GL rt)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

