This is a project to port the game to iOS and macOS devices.

## Current progress

Rendering terrain of crash sector and a player's moving character, demo video:

[![Game running on iOS](https://img.youtube.com/vi/zyBsQ3fYTZQ/hqdefault.jpg)](https://youtu.be/zyBsQ3fYTZQ)

## How to run locally

- Clone this repository
- Open this project in XCode
- Connect your physical iOS device and select it as a target device in XCode.

<img width="641" alt="xcode-ios-target-device" src="https://user-images.githubusercontent.com/48682076/227014531-24c6db20-d85e-4dee-a79b-14703374b713.png">

- Build and run the project.
- The app will fail to launch and that is expected. You will also need to explicitly trust the developer in the settings of your device. Simply follow the instructions on your device screen.

Currently we support only iOS devices. iOS simulator cannot run it because we are using indirect command buffers on GPU - and simulator simply does not support them.

#### External dependencies

All third-party includes are provided with this source code. You should not have to do any extra work there.

For the information - we currently depend on 3 libraries:
- Apple's metal-cpp library for macOS12/iOS15. [Link](https://developer.apple.com/metal/cpp/).
- GLM library v0.9.9.8. [Link](https://github.com/g-truc/glm).
- C++ Boost library v1.79.0. [Link](https://www.boost.org/doc/libs/1_79_0/).

#### Game assets

We are against game piracy, and therefore the original game assets are not included in this repository. In order to run the project, you must include them yourself after extracting them from the original game files. Below is the list of the assets currently required.

- .art files for tiles. Add all tiles' .art files to `/opt/Arcanum Revitalized/tile` folder. **Important**: all file names must be in lowercase. To achieve this, you can run a script from the most popular answer here: https://stackoverflow.com/questions/7787029/how-do-i-rename-all-files-to-lowercase

## Contributors

- AxelStrem: provided a converter from .art to .bmp. Original project is here: https://github.com/AxelStrem/ArtConverter
