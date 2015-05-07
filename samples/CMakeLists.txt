
find_package(X11)

if(!ANDROID)
	find_package(OpenGL)
endif(!ANDROID)

add_subdirectory(basic)
add_subdirectory(dynamic)
add_subdirectory(gesture)
add_subdirectory(listener)
add_subdirectory(recording)
add_subdirectory(sync)

