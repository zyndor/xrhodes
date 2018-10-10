[XRhodes](https://github.com/zyndor/xrhdoes) - Cross-platform game development / creative coding framework

[![License](https://img.shields.io/badge/license-BSD--2%20clause-blue.svg)](https://zyndor.github.io/xrhodes/license.html)

XRhodes is a game development / creative coding framework delivered as a set of static libraries. It's aimed at developers who seek more control and less complicated licensing terms than popular engines offer and is best suited at small to medium sized games.

Its aim is to provide flexible tools, rather than a complete pipeline, while abstracting most platform-specific details away.

XRhodes is written in modern C++, and it currently supports Windows 8+ and Mac OS X; support for other platforms are in the pipeline - stay tuned!

It comes with a fair amount of documentation and with unit tests and usage examples.

Main features
-------------

* Signals and generic callbacks;
* Tweening;
* Lighter-than-JSON-weight text-based serialization format;
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

Getting started
---------------

Visit the tools/ subfolder for your chosen development platform and run the generate_projects script. Alternatively, you can run premake directly with the premake5.lua script in the root folder.

Projects are generated in the .projects folder in the root.

Run examples and unit tests, study the project configuration files, create your own projects.

Happy building!

Community
---------

The [XRhodes Discord channel](https://discord.gg/kCQ9NQ6) welcomes questions and discussion. This may be a faster option than filing a bug report.

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
