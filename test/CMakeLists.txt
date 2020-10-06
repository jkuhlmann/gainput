
project(gainputtest)

file(GLOB_RECURSE sources *.cpp)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

if(ANDROID)
	add_library(gainputtest SHARED ${sources})
else()
	add_executable(gainputtest ${sources})
endif()

if(APPLE AND NOT IOS)
	find_library(APPKIT AppKit)
else()
	set(APPKIT "")
endif()

target_include_directories(gainputtest PRIVATE ../extern/catch/)
target_link_libraries(gainputtest gainput)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(gainputtest X11 GL rt)
endif()

if(BUILD_SHARED_LIBS)
  # Copy the output of gainput shared library to output folder.
  add_custom_command( TARGET gainputtest 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:gainput> $<TARGET_FILE_DIR:gainputtest>
  )
endif(BUILD_SHARED_LIBS)
