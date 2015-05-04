
project(gainputtest)

include_directories(../lib/include/)
include_directories(../extern/catch/)

file(GLOB_RECURSE sources *.cpp)

if(ANDROID)
	add_library(gainputtest SHARED ${sources})
else()
	add_executable(gainputtest ${sources})
endif()

target_link_libraries(gainputtest gainputstatic)


if(ANDROID)
	target_link_libraries(gainputtest c stdc++ android m gnustl_static)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	find_package(X11)
	find_package(OpenGL)
	target_link_libraries(gainputtest X11 GL rt)
elseif(WIN32)
	target_link_libraries(gainputtest xinput ws2_32)
endif()

