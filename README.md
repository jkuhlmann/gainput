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

By default, Gainput is built using [Waf](http://code.google.com/p/waf/). So make sure you have recent version of [Python](http://www.python.org/) installed.

### Windows & Linux

1. Run `waf configure`
1. Run `waf build_debug` or `waf build_release`
1. The executables should be in `build/debug/` or `build/release/` respectively.

### Android NDK

Building for Android is little more complicated at the moment, but here you go:

1. Run `waf configure --cross-android --cross-android-ndk=ANDROID_NDK_PATH`
1. Run `waf build_debug` or `waf build_release`
1. Do `cp build/debug/libbasicsample.so samples/android/libs/armeabi/`
1. Do `cd samples/android/`
1. Run `ANDROID_SDK_PATH/tools/android update project -p . -s --target android-17`
1. Run `ant debug`
1. Run `ANDROID_SDK_PATH/platform-tools/adb install bin/Gainput-simple-debug.apk`

Replace `ANDROID_NDK_PATH` and `ANDROID_SDK_PATH` with your local paths.

Alternatives
------------

- [OIS](https://github.com/wgois/Object-oriented-Input-System--OIS-)
- [SDL](http://www.libsdl.org/)


