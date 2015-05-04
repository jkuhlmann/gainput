
project(gainput)

include_directories (include/)

if(ANDROID)
	include_directories(${ANDROID_NDK}/platforms/android-9/arch-arm/usr/include/)
endif()

file(GLOB_RECURSE sources source/*.cpp)

add_library(gainputstatic STATIC ${sources})

add_library(gainput SHARED ${sources})

if(WIN32)
	target_link_libraries(gainput xinput ws2_32)
	add_definitions(-DGAINPUT_LIB_DYNAMIC=1)
elseif(ANDROID)
	target_link_libraries(gainputstatic native_app_glue log android)
	target_link_libraries(gainput native_app_glue log android)
endif()

