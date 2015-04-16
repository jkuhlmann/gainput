
project(basicsample)

include_directories(../../lib/include/)

file(GLOB_RECURSE sources *.cpp)

add_executable(basicsample ${sources})

target_link_libraries(basicsample gainputstatic)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(basicsample X11 GL rt)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")

