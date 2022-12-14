This is a project to port the game to iOS and macOS devices.

## Current progress

Rendering terrain of crash sector and a player's character, demo video:

[![Game running on iOS](https://img.youtube.com/vi/_essxtbsGzo/hqdefault.jpg)](https://www.youtube.com/watch?v=_essxtbsGzo)

## How to run locally

Currently support only iOS devices.

#### External dependencies

To build the project, you must ensure that you have project dependencies in place. Currently we depend on 3 libraries:
- Apple's metal-cpp library for macOS12/iOS15. Download [here](https://developer.apple.com/metal/cpp/). Specify the path to the library in `Header Search Paths` in build settings. For example: `/opt/metal/metal-cpp`.
- GLM library v0.9.9.8. Download from releases [here](https://github.com/g-truc/glm). Specify the path to the library in `Header Search Paths` in build settings. For example: `/opt/glm`.
- C++ Boost library v1.79.0. Install using homebrew. Specify the path to the library in `Header Search Paths` in build settings. For example: `/opt/homebrew/Cellar/boost/1.79.0/include`.

#### Game assets

We are against game piracy, and therefore the original game assets are not included in this repository. In order to run the project, you must include them yourself after extracting them from the original game files. Below is the list of the assets currently required.

- .art files for tiles. Add all tiles' .art files to `/opt/Arcanum Revitalized/tile` folder. **Important**: all file names must be in lowercase. To achieve this, you can run a script from the most popular answer here: https://stackoverflow.com/questions/7787029/how-do-i-rename-all-files-to-lowercase

To run the project, you must connect the physical iOS device. iOS simulator cannot run it because of the rendering techniques used - simulator simply does not support them yet.

## Contributors

- AxelStrem: provided a converter from .art to .bmp. Original project is here: https://github.com/AxelStrem/ArtConverter
