
project(recordingsample)

file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE sfwsources ../samplefw/*.cpp)

if(ANDROID)
	add_library(recordingsample SHARED ${sources} ${sfwsources})
else()
	add_executable(recordingsample WIN32 ${sources} ${sfwsources})
endif()

target_include_directories(recordingsample PRIVATE ../samplefw/)
target_link_libraries(recordingsample gainput)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(recordingsample X11 GL rt)
endif()

if(BUILD_SHARED_LIBS)
  # Copy the output of gainput shared library to output folder.
  add_custom_command( TARGET recordingsample 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:gainput> $<TARGET_FILE_DIR:recordingsample>
  )
endif(BUILD_SHARED_LIBS)
