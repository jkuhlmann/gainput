
project(dynamicsample)

include_directories(../samplefw/)

file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE sfwsources ../samplefw/*.cpp)

if(ANDROID)
	add_library(dynamicsample SHARED ${sources} ${sfwsources})
else()
	add_executable(dynamicsample WIN32 ${sources} ${sfwsources})
endif()

target_include_directories(dynamicsample PRIVATE ../samplefw/)
target_link_libraries(dynamicsample gainput)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(dynamicsample X11 GL rt)
endif()

if(BUILD_SHARED_LIBS)
  # Copy the output of gainput shared library to output folder.
  add_custom_command( TARGET dynamicsample 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:gainput> $<TARGET_FILE_DIR:dynamicsample>
  )
endif(BUILD_SHARED_LIBS)

