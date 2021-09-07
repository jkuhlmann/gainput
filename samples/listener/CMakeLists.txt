
project(listenersample)

file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE sfwsources ../samplefw/*.cpp)

if(ANDROID)
	add_library(listenersample SHARED ${sources} ${sfwsources})
else()
	add_executable(listenersample WIN32 ${sources} ${sfwsources})
endif()

target_include_directories(listenersample PRIVATE ../samplefw/)
target_link_libraries(listenersample gainput)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(listenersample X11 GL rt)
endif()

if(BUILD_SHARED_LIBS)
  # Copy the output of gainput shared library to output folder.
  add_custom_command( TARGET listenersample 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:gainput> $<TARGET_FILE_DIR:listenersample>
  )
endif(BUILD_SHARED_LIBS)
