# Skylicht Engine
## About
Skylicht Engine, which is an evolution of [Irrlicht Project](http://irrlicht.sourceforge.net). We have upgraded more feature: Sound Engine, Physics Engine, Particle Engine...

-   Core functionality: Direct3D11, OpenGL 4+, OpenGLES 3+, Google Angle GLES video driver backends.

-   Cross-Platform Framework: Android, IOS, Win32/Win64, UWP, Linux, MacOS, HTML5.

-   Render pipeline: Forwarder & Deferred.


Skylicht Engine is a super lightweight Game Engine, that target to mobile platform (Android, IOS). And, it's completely free.

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/6d6fbf50a10a4cf38426b9fabfc1fabc)](https://www.codacy.com/manual/ducphamhong/skylicht-engine?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=skylicht-lab/skylicht-engine&amp;utm_campaign=Badge_Grade)

## How To Build
| Platform                                                                                 | Build Status                                                                                                                                                                      |
| -----------------------------------------------------------------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| <img src="Documents/Media/Platforms/windows.png" width="25" valign="middle"> Windows     | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| <img src="Documents/Media/Platforms/cygwin.png" width="25" valign="middle"> Cygwin64     | [![Build status](https://ci.appveyor.com/api/projects/status/a95huci4g5o25ts5/branch/master?svg=true)](https://ci.appveyor.com/project/ducphamhong/skylicht-engine/branch/master) |
| <img src="Documents/Media/Platforms/uwp.png" width="25" valign="middle"> Windows Store   | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/apple.png" width="25" valign="middle"> MacOS         | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/ubuntu.png" width="25" valign="middle"> Linux        | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/html5.png" width="25" valign="middle"> Emscripten    | [![Build status](https://github.com/skylicht-lab/skylicht-engine/workflows/build/badge.svg)](https://github.com/skylicht-lab/skylicht-engine/actions?workflow=build)              |
| <img src="Documents/Media/Platforms/android.png" width="25" valign="middle"> Android     | [![Build status](https://circleci.com/gh/skylicht-lab/skylicht-engine.svg?style=shield)](https://circleci.com/gh/skylicht-lab/skylicht-engine/tree/master)                        |

### Prerequisites
-   [CMake](https://cmake.org/download/) 3.12 or higher
-   [Python](https://www.python.org/downloads/) 3.x
-   [Visual Studio](https://visualstudio.microsoft.com/downloads/)  (2017 or higher if you want to build on Windows OS)
### Build Command
-   Clone this repo

-   Build assets bundle
    ```console
    C:\skylicht-engine>cd Assets
    C:\skylicht-engine\Assets>python BuildAssetBundles.py
    C:\skylicht-engine\Assets>cd ..
    ```
    
-   Run CMake from the current directory to generate visual studio project or xcode project
    ```console
    # Visual Studio 2017
    C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
  
    # Visual Studio 2019
    C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 16 2019" -A x64
    
    # Xcode
    /User/Skylicht/Documents/skylicht-engine$cmake -S . -B ./PrjMac -G Xcode 
    ```
-   Open the generated solution `PrjVisualStudio/SKYLICHT_ENGINE.sln`

-   Once the solution opens, right click the **SampleXXX** project, click **"Set as StartUp Project"** and click the play button at the top to run the Demo App.

-   More details: please preview command in **BuildCommand** folder.

### Build Compress Texture, Shader

-   This python tool will convert texture .TGA to compressed texture (.DDS, .ETC2, .PVRTC) to optimize gpu memory on runtime. (**Optional**, just rebuild when you modify/add texture to asset)
    ```console
    # Install Tinydb
    # https://pypi.org/project/tinydb
    C:\skylicht-engine>C:\Python37\Scripts\pip install tinydb

    # Install Pillow (Image processing)
    # https://pillow.readthedocs.io/en/4.1.x/index.html
    C:\skylicht-engine>C:\Python37\Scripts\pip install Pillow

    # Compress TGA to DDS, ETC2, PVR texture
    C:\skylicht-engine>cd Assets    
    C:\skylicht-engine\Assets>python BuildTextureCompressDDS.py
    C:\skylicht-engine\Assets>python BuildTextureCompressETC.py
    C:\skylicht-engine\Assets>python BuildTextureCompressPVR.py
    C:\skylicht-engine\Assets>cd ..
    ```
    
-   This python tool use C Preprocessor to inline shader script (HLSL, HLSL) (**Optional**, just rebuild when you modify built-in shader)
    ```console
    # Install pcpp (C Preprocessor tool)
    # https://pypi.org/project/pcpp
    C:\skylicht-engine>C:\Python37\Scripts\pip install pcpp
    
    # Build shader script
    C:\skylicht-engine>cd Assets
    C:\skylicht-engine\Assets>python BuildShader.py
    C:\skylicht-engine\Assets>cd ..
    ```
    
### Add your code to  Project

#### Add source code
- Add new source files or subfolders on `Projects/{ProjectName}/Source` and regenerate project
    ```console
    C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
    ```

#### Create application project
- Run **Scripts/create_project.py**. Example: Create application *NewApplication* at folder *Samples\NewApplication*
```console
    C:\skylicht-engine>python Scripts\create_project.py NewApplication Samples\NewApplication
```
- Edit **CMakeProjects.cmake**, add new line:"*subdirs (Samples/NewApplication)*" and regenerate projects
```console
    C:\skylicht-engine>cmake -S . -B ./PrjVisualStudio -G "Visual Studio 15 2017" -A x64
```
- Open Visual Studio Solution and click **NewApplication** - **"Set as StartUp Project"**.

### Samples\HelloWorld
Engine Components are used:
- **FreeType Font**
- **Glyph Font**
- **Canvas Component**
- **Graphics2D**
- **GUI Text**
<img src="Documents/Media/Samples/sample_hello_world.png"/>

### Samples\LuckyDraw
Engine Components are used:
- **FreeType Font**
- **Glyph Font**
- **Graphics2D**
- **GUI Text**
- **GUI Image**
- **GUI Mask**
- **EventManager**
<img src="Documents/Media/Samples/sample_lucky_draw.gift"/>

### Samples\TankScene
Engine Components are used:
- **OBJ Wavefront** loader
- **Deferred** and **Forwarder** rendering
- **Dynamic Direction Lighting**
- **Shadow Mapping**
- **Lightmapper**
- **Skydome**

#### Direction Light & Shadow Mapping (1 bounce)
<img src="Documents/Media/Samples/tank-scene/direction-1-bounce.png"/>

#### Baked Global Illumination (2 bounces)
Irradiance baked on Vertex Color
<img src="Documents/Media/Samples/tank-scene/indirect-bake-2-bounce.png"/>

**Final Composition**
<img src="Documents/Media/Samples/tank-scene/combine-2-bounces.png"/>

Lightmapper works well on Wasm (OpenGLES3.0)
<img src="Documents/Media/Samples/tank-scene/emsdk-2-bounces.png"/>

#### **Final Composition** (3 bounces)

<img src="Documents/Media/Samples/tank-scene/combine-3-bounces-a.png"/>

<img src="Documents/Media/Samples/tank-scene/combine-3-bounces-b.png"/>

## Contributing
Welcome if you want to contribute your code to **Skylicht Engine** (Ex: add feature or fix bug). Wellcome to join group. Open an issues and submit a pull request to this repository. Please read [CONTRIBUTING.md](Documents/CONTRIBUTING.md) for details on our code of conduct, and the process for submitting pull requests.

## License
Skylicht Engine is licensed under [MIT License](LICENSE.md)

Skylicht Engine is based in part on the work of:
-   [Irrlicht Engine](http://irrlicht.sourceforge.net)
-   [Bullet Physics](https://github.com/bulletphysics/bullet3)
-   [Google Angle](https://github.com/google/angle)
-   [Ocornut Imgui](https://github.com/ocornut/imgui)
-   [Xatlas](https://github.com/jpcy/xatlas)
-   [Spark Particle](https://github.com/Synxis/SPARK)
-   [Freetype2](http://git.savannah.gnu.org/cgit/freetype/freetype2.git)
-   Independent JPEG Group, libPng, zlib, curl...

This means that if you've used the Skylicht Engine in your product, you must acknowledge somewhere in your documentation that you've used. It would also be nice to mention that you use the 3rd parties library... Please see the README files from 3rd parties for further informations.
