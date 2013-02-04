Gainput
=======

Gainput is an input library for games:

- written in C++
- clean codebase
- easy to use
- cross-platform
  - Windows
  - Linux
  - Android NDK
- support for different devices
  - keyboard
  - mouse
  - pad
- open source (MIT license)

Gainput is currently in development and **should NOT be used yet**.

Building
--------

Windows, Linux:

1. Make sure you have python installed.
1. Run waf configure.
1. Run waf build\_debug or waf build\_release.
1. The executables should be in build/debug/ or build/release/ respectively.

Android:

1. waf configure --cross-android --cross-android-ndk=.../android-ndk-r8d/
1. waf build\_debug or waf build\_release
1. cp build/debug/libbasicsample.so samples/android/libs/armeabi/
1. cd samples/android/
1. android-sdk-linux/tools/android update project -p . -s --target android-17
1. ant debug
1. android-sdk-linux/platform-tools/adb install bin/Gainput-simple-debug.apk

Alternatives
------------

- [OIS](https://github.com/wgois/Object-oriented-Input-System--OIS-)
- [SDL](http://www.libsdl.org/)


