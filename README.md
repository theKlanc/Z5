# Z5
A 2.5D Voxel game engine with 4 degrees of freedom, with support for realistic universe-scale distances without loss of precision.
It includes 2-body orbit simulations, with a 1 to 1 scale version of the solar system (with disc shaped celestial bodies)

![game image](https://github.com/theKlanc/Z5/blob/master/resources/cap.png?raw=true)

[DEMO VIDEO](https://youtu.be/hTAyvlIEYB8)


Is currently able to compile to x86-64 targets on Linux and Windows. Also compiles to Nintendo Switch homebrew and Emscripten.
To try it just clone the repo and compile the makefile for your target (use emmake for em.mk, and compile on Windows using Visual Studio / MSVC)
For a demo just visit [z5.ledgedash.com](https://z5.ledgedash.com), but emscripten builds are experimental, ymmv.

## Special Thanks to:
* [Jordan Pecks (Auburns)](https://github.com/Auburns) For his [FastNoise c++ Library](https://github.com/Auburns/FastNoise)
* [Michele Caini (skypjack)](https://github.com/skypjack) and all other contributors to the [EnTT project](https://github.com/skypjack/entt)
* Xerpi, for helping me when starting to work on 3DS homebrew
* The Atmosphère and Fail0verflow teams, for making homebrew on the Switch possible
