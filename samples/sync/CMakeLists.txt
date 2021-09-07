
project(syncsample)

file(GLOB_RECURSE sources *.cpp)
file(GLOB_RECURSE sfwsources ../samplefw/*.cpp)

if(ANDROID)
	add_library(syncsample SHARED ${sources} ${sfwsources})
else()
	add_executable(syncsample WIN32 ${sources} ${sfwsources})
endif()

target_include_directories(syncsample PRIVATE ../samplefw/)
target_link_libraries(syncsample gainput)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(syncsample X11 GL rt)
endif()

if(BUILD_SHARED_LIBS)
  # Copy the output of gainput shared library to output folder.
  add_custom_command( TARGET syncsample 
    POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:gainput> $<TARGET_FILE_DIR:syncsample>
  )
endif(BUILD_SHARED_LIBS)
