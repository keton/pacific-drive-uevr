# UEVR Compatibility Plugin for Pacific Drive

## Features
* Fixes in-car camera. Set your aiming method to left or right controller and then you can rotate the view inside the car using motion.
* Car will start and be put to drive when entering, stop and put to park on exiting
* Handheld items bound to (left/right) motion controller

## Building

1. Install Visual Studio 2022 with C++, Windows SDK and CMake support
1. Make sure you follow all instructions [here](https://github.com/praydog/UEVR/blob/master/COMPILING.md) and are able to build UEVR from source
1. Clone this repository
1. Run `build.cmd` or in VS Developer Shell:
	```shell
	cmake -S . -B build ./build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
	cmake --build ./build --clean-first --config Release --target pacific_drive_plugin
	```
1. copy resulting `pacific_drive_plugin.dll` to your game profile `plugins` subfolder
