[XRhodes](https://github.com/zyndor/xrhdoes) - Cross-platform game development / creative coding framework

[![License](https://img.shields.io/badge/license-BSD--2%20clause-blue.svg)](https://zyndor.github.io/xrhodes/license.html)

XRhodes is a game development / creative coding framework delivered as a set of static libraries. It's aimed at developers who seek more control and less complicated licensing terms than popular engines offer, and is best suited at small to medium sized games.

Its aim is to provide flexible tools, rather than a complete pipeline, while abstracting most platform-specific details away.

XRhodes is written in C++14, and it currently supports Windows 8+ (32 / 64 bits), Mac OS and Linux (64 bits); support for other platforms are in the pipeline - stay tuned!

It comes with a fair amount of documentation and with unit tests and usage examples.

Main features
-------------

* Signals and generic callbacks;
* Tweening;
* Lighter-than-JSON-weight text-based serialization format and structured reading;
* JSON parsing and writing;
* Vector maths library;
* Low level graphics API (Current implementation is OpenGL based);
* Asset management system with support for dependencies, offline building and asynchronous loading;
* Texture pack support;
* Signed Distance Field fonts support with UTF-8 text;
* Mouse, touch, keyboard and game controller support;
* Entity / component system;

Structure
---------

XRhodes is made up from the following libraries:

* xr3core: low level, platform-independent utilities with no external dependencies;
* xr3json: XRhodes' JSON parsing and writing library for both event-based parsing and building a DOM-tree;
* xr3: device abstraction, file system, input, graphics, asset management, meshes, sprites, text and other higher level facilities;
* xr3scene: entity / component system;

System Requirements
-------------------

Graphics: OpenGL3.3 capable GPU.


# Windows

- Visual Studio Community 2017+ and (Vcpkg)[https://github.com/microsoft/vcpkg]
- $ ./vcpkg install --triplet=x86-windows libpng sdl2 tinyxml2 gtest
- $ ./vcpkg install --triplet=x64-windows libpng sdl2 tinyxml2 gtest

# MacOS 10.13+

- Xcode 4+
- $ brew install libpng sdl2 tinyxml2

# Linux (Tested under Ubuntu 18.04)

- $ sudo apt-get install build-essential mesa-common-dev mesa-utils libpng-dev \
  libsdl2-dev libtinyxml2-dev

Getting started
---------------

Visit the tools/ subfolder for your chosen development platform and run the
generate_projects script. Alternatively, you can run premake directly with the
premake5.lua script in the root folder.

Projects are generated in the .projects folder in the root. Run the build_all
script from the same folder, or build the projects manually.

Run examples and unit tests.

Creating your own projects
--------------------------

1. Set an 'XR3_PATH' environment variable with the location of XRhodes.
2. Copy and rename the template/ folder to your desired location.
3. Edit the config.lua file with your desired settings for project name, files
  to include in the build, and external includes location.
4. Run premake with the premake5.lua script.

Happy building!

Community
---------

The [XRhodes Discord channel](https://discord.gg/kCQ9NQ6) welcomes questions and
discussion. This may be a faster option than filing a bug report.

If you find XRhodes useful...
-----------------------------

...please consider supporting its development. You can do a lot with very little:

* read CONTRIBUTORS.md on contributing to the code base;
* consider making a [regular donation](https://patreon.com/zyndor);
* consider making a [one-off donation](https://paypal.me/zyndor);
* display a logo in your product / project or website;
* tell people about XRhodes;

Thank You!

Acknowledgements
----------------

* [premake](http://industriousone.com/premake) - build configuration tool;
* [SDL2](https://www.libsdl.org/) - the desktop version of XRhodes would not exist without it;
* [glLoadGen](https://bitbucket.org/alfonse/glloadgen/wiki/Home) - fantastic OpenGL header generator with much friendlier licensing terms than its alternatives;
* [bgfx](https://github.com/bkaradzic/bgfx) - major inspiration for the graphics API;
* [stb_truetype](https://github.com/nothings/stb/blob/master/stb_truetype.h) - TTF support;
* [utf8cpp](http://utfcpp.sourceforge.net/) - concise and easy-to-use UTF8 library;
* [tinyxml2](http://www.grinninglizard.com/tinyxml2/) - XML parsing;
* [MurmurHash](https://github.com/aappleby/smhasher) - powers xr::Hash;
