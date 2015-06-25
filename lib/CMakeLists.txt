
project(gainput)

set(CMAKE_MACOSX_RPATH 1)

include_directories (include/)

file(GLOB_RECURSE sources source/*.cpp)

if(APPLE)
	file(GLOB_RECURSE mmsources source/*.mm)
endif()

add_library(gainputstatic STATIC ${sources} ${mmsources})

add_library(gainput SHARED ${sources} ${mmsources})

if(WIN32)
	target_link_libraries(gainput ${XINPUT} ws2_32)
	add_definitions(-DGAINPUT_LIB_DYNAMIC=1)
elseif(ANDROID)
	target_link_libraries(gainputstatic native_app_glue log android)
	target_link_libraries(gainput native_app_glue log android)
elseif(APPLE)
	find_library(FOUNDATION Foundation)
	find_library(IOKIT IOKit)
	find_library(APPKIT AppKit)
	target_link_libraries(gainput ${FOUNDATION} ${IOKIT} ${APPKIT})
endif()

