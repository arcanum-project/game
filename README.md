This is a project to port the game to iOS and macOS devices.

### Current progress

Rendering terrain of crash sector, demo:

[![Game running on iOS](https://img.youtube.com/vi/20a8qGrUdn8/hqdefault.jpg)](https://youtu.be/20a8qGrUdn8)

### How to run locally

Currently support only iOS devices.

To build the project, you must ensure that you have project dependencies in place. Currently we depend on 3 libraries:
- Apple's metal-cpp library for macOS12/iOS15. Download [here](https://developer.apple.com/metal/cpp/). Specify the path to the library in `Header Search Paths` in build settings. For example: `/opt/metal/metal-cpp`.
- GLM library v0.9.9.8. Download from releases [here](https://github.com/g-truc/glm). Specify the path to the library in `Header Search Paths` in build settings. For example: `/opt/glm`.
- C++ Boost library v1.79.0. Install using homebrew. Specify the path to the library in `Header Search Paths` in build settings. For example: `/opt/homebrew/Cellar/boost/1.79.0/include`.

To run the project, you must connect the physical iOS device. iOS simulator cannot run it because of the rendering techniques used - simulator simply does not support them yet.
