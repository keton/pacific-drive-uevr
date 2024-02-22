# UEVR Plugin for Pacific drive demo

## Features
* Fixes in-car camera. Set your aiming method to left or right controller and the you can rotate the view inside the car using motion.
* car will start and be put to drive when entering, stop and put to park on exiting
* hand held items bound to (left/right) motion controller

## Known issues

If you travel while in car or load a save when you are inside **you need to exit and enter the car** before in-car camera will work.
This is because game function we use - `getPlayerInCarPercent()`has a bug and returns 0 instead of 1 in those conditions.

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
