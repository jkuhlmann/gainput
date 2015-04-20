
project(gainput)

include_directories (include/)

file(GLOB_RECURSE sources source/*.cpp)

add_library(gainputstatic STATIC ${sources})

add_library(gainput SHARED ${sources})

if(WIN32)
	target_link_libraries(gainput xinput ws2_32)
	add_definitions(-DGAINPUT_LIB_DYNAMIC=1)
endif()
