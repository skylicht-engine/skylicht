# Skylicht Engine

## About

Skylicht Engine is a fork from [Irrlicht 3D Engine](http://irrlicht.sourceforge.net), that we have upgraded more feature: Sound Engine, Physics Engine, Particle Engine...

  - Core functionality: Direct3D11, OpenGL 3+, OpenGLES 3+ video driver backends.
  - Cross-Platform Framework: Android, IOS, Win32/Win64, UWP, Linux, MacOS, HTML5.
  - Render pipeline: Forwarder & Deferred.

Skylicht Engine is a super lightweight Game Engine, that target to mobile platform (Android, IOS). And, it's completely free.

| Platform    | Build Status                                                                                                                                                                               |
| ------------|--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| Win32/Win64 | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master)          |
| Cygwin64    | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master)          |
| Android     | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master)          |
| Linux       | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/C%2FC%2B%2B%20CI/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)            |

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/6d6fbf50a10a4cf38426b9fabfc1fabc)](https://www.codacy.com/manual/ducphamhong/skylicht-engine?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=skylicht-lab/skylicht-engine&amp;utm_campaign=Badge_Grade)

## Getting Started

### Windows

#### Prerequisites
  - [CMake](https://cmake.org/download/) 3.12 or higher
  - [Python](https://www.python.org/downloads/) 3.x
  - [Visual Studio](https://visualstudio.microsoft.com/downloads/) 2017 (or higher)

#### How To Build

  - Clone this repo.
  - Update the submodules.
    ```
    C:\skylicht-engine>git submodule update --init --recursive
    ```
  - Create a new directory for the build files, e.g. `Build` at the root of the repo.
    ```
    C:\skylicht-engine>mkdir PrjVisualStudio
    ```
  - Change your working directory to the new directory.
    ```
    C:\skylicht-engine>cd PrjVisualStudio
    ```
  - Run CMake from the new directory and point to the root of the repo.  
    ```
    C:\skylicht-engine\Build>cmake ..
    ```
  - Open the generated solution `SKYLICHT_ENGINE.sln`.
  - Once the solution opens, right click the MainApp project, click "Set as StartUp Project" and click the play button at the top to run the Demo App.

## License

Skylicht Engine is under [MIT License](LICENSE.md).

Skylicht Engine is based in part on the work of:
  - Irrlicht Engine
  - Bullet Physics
  - Independent JPEG Group
  - zlib
  - curl 
  - libPng  

This means that if you've used the Skylicht Engine in your product, you must acknowledge somewhere in your documentation that you've used. It would also be nice to mention that you use the Irrlicht Engine, Bullet Physics ,the zlib, curl and libPng... Please see the README files from 3rd parties for further informations.