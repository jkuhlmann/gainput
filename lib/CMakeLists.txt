project(gainput)
message(STATUS "GAINPUT version ${GAINPUT_VERSION}")

set(CMAKE_MACOSX_RPATH 1)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++98 -Wall -Wextra -pedantic -Wshadow -Wno-variadic-macros")
endif()

file(GLOB_RECURSE sources source/*.cpp source/*.h include/*.h)

if(APPLE)
  file(GLOB_RECURSE mmsources source/*.mm)
endif()

add_library(gainput ${sources} ${mmsources})
set_target_properties(gainput PROPERTIES
  OUTPUT_NAME gainput
  DEBUG_POSTFIX -d
  VERSION ${GAINPUT_VERSION}
  SOVERSION ${GAINPUT_MAJOR_VERSION}
  FOLDER gainput
)
set(install_libs ${install_libs} gainput)

target_include_directories(gainput PUBLIC include)

if(WIN32)
  target_link_libraries(gainput PUBLIC ${XINPUT} ws2_32)
  if(BUILD_SHARED_LIBS)
    target_compile_definitions(gainput PRIVATE -DGAINPUT_LIB_DYNAMIC=1)
#    target_compile_definitions(gainput INTERFACE -DGAINPUT_LIB_DYNAMIC_USE=1)
  endif(BUILD_SHARED_LIBS)
elseif(ANDROID)
  target_link_libraries(gainput PUBLIC native_app_glue log android)
elseif(APPLE)
  find_library(FOUNDATION Foundation)
  find_library(IOKIT IOKit)
  find_library(GAME_CONTROLLER GameController)
  target_link_libraries(gainput PUBLIC ${FOUNDATION} ${IOKIT} ${GAME_CONTROLLER})
  if(IOS)
    find_library(UIKIT UIKit)
    find_library(COREMOTION CoreMotion)
    find_library(QUARTZCORE QuartzCore)
    target_link_libraries(gainput PUBLIC ${UIKIT} ${COREMOTION})
  else()
    find_library(APPKIT AppKit)
    target_link_libraries(gainput PUBLIC ${APPKIT})
  endif()
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
