
project(listenersample)

include_directories(../../lib/include/)
include_directories(../samplefw/)

file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE sfwsources ../samplefw/*.cpp)

add_executable(listenersample ${sources} ${sfwsources})

target_link_libraries(listenersample gainputstatic)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(listenersample X11 GL rt)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

