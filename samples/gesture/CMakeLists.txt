
project(gesturesample)

include_directories(../../lib/include/)
include_directories(../samplefw/)

file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE sfwsources ../samplefw/*.cpp)

add_executable(gesturesample ${sources} ${sfwsources})

target_link_libraries(gesturesample gainputstatic)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(gesturesample X11 GL rt)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

