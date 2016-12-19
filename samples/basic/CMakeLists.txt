
project(basicsample)

include_directories(../../lib/include/)

file(GLOB_RECURSE sources *.cpp)

if(APPLE)
    file(GLOB_RECURSE mmsources *.mm)
endif()

if(ANDROID)
	add_library(basicsample SHARED ${sources})
else()
	add_executable(basicsample WIN32 ${sources} ${mmsources})
endif()

target_link_libraries(basicsample gainputstatic)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
	target_link_libraries(basicsample X11 GL rt)
elseif(WIN32)
	target_link_libraries(basicsample ${XINPUT} ws2_32)
elseif(APPLE)
    target_link_libraries(basicsample ${FOUNDATION} ${IOKIT} ${APPKIT})
endif()

