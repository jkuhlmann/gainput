
project(gainput)

include_directories (include/)

file(GLOB_RECURSE sources source/*.cpp)

add_library(gainputstatic STATIC ${sources})

add_library(gainput SHARED ${sources})

