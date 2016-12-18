cmake_minimum_required(VERSION 2.8)
set(GAINPUT_MAJOR_VERSION 0)
set(GAINPUT_MINOR_VERSION 10)
set(GAINPUT_PATCH_VERSION 0)
set(GAINPUT_VERSION ${GAINPUT_MAJOR_VERSION}.${GAINPUT_MINOR_VERSION}.${GAINPUT_PATCH_VERSION})

project(gainput)
message(STATUS "GAINPUT version ${GAINPUT_VERSION}")

set(CMAKE_MACOSX_RPATH 1)

if(CMAKE_COMPILER_IS_GNUCXX)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -Wall -Wextra -pedantic -Wshadow -Wno-variadic-macros")
endif()

include_directories (include/)

file(GLOB_RECURSE sources source/*.cpp)

if(APPLE)
	file(GLOB_RECURSE mmsources source/*.mm)
endif()

## build STATIC *or* SHARED
option(GAINPUT_BUILD_SHARED "BUILD_SHARED" OFF)
if (GAINPUT_BUILD_SHARED)
  add_library(gainput_shared SHARED ${sources} ${mmsources})
  set_target_properties(gainput_shared PROPERTIES
    OUTPUT_NAME gainput
    DEBUG_POSTFIX -d
    VERSION ${GAINPUT_VERSION}
    SOVERSION ${GAINPUT_MAJOR_VERSION}
    FOLDER gainput
  )
  set(install_libs gainput_shared)
else()
  message(STATUS "## Building STATIC libraries (-DGAINPUT_BUILD_SHARED=ON to build shared libraries)")
  add_library(gainput STATIC ${sources} ${mmsources})
  set_target_properties(gainput PROPERTIES DEBUG_POSTFIX -d FOLDER gainput)
  set(install_libs gainput)
endif (GAINPUT_BUILD_SHARED)

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

# Library installation directory
if(NOT DEFINED CMAKE_INSTALL_LIBDIR)
    set(CMAKE_INSTALL_LIBDIR lib)
endif(NOT DEFINED CMAKE_INSTALL_LIBDIR)
set(libdir ${CMAKE_INSTALL_PREFIX}/${CMAKE_INSTALL_LIBDIR})

install(
    DIRECTORY "include/gainput"
    DESTINATION "include"
    FILES_MATCHING PATTERN "*.h"
)

install(
    TARGETS ${install_libs}
    LIBRARY DESTINATION "${libdir}"
    ARCHIVE DESTINATION "${libdir}"
    RUNTIME DESTINATION "bin"
)

