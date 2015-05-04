cmake_minimum_required(VERSION 2.8)

if(!WIN32)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -Wextra")
endif()

if(ANDROID)
	include(extern/cmake/AndroidNdkModules.cmake)
	android_ndk_import_module_native_app_glue()
endif()

add_subdirectory(lib)
add_subdirectory(samples)
add_subdirectory(test)


