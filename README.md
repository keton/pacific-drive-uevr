# UEVR Plugin for Pacific drive demo

__Work In Progress__ not compatible with the full game. Expect problems. Works fine with demo.

## Features
* When UEVR is set to left or right controller aiming method in-car aiming works correctly
* car will start and be put to drive when entering, stop and put to park on exiting
* hand held items bound to (left/right) motion controller

## Building

1. Install Visual Studio 2022 with C++, Windows SDK and CMake support
1. Make sure you follow all instructions [here](https://github.com/praydog/UEVR/blob/master/COMPILING.md) and are able to build UEVR from source
1. Clone this repository
1. Run `build.cmd` or in VS Developer Shell:
	```shell
	cmake -S . -B build ./build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
	cmake --build ./build --clean-first --config Release --target pacific_drive_plugin
	```
1. copy resulting pacific_drive_plugin.dll to your game profile `plugins` subfolder
