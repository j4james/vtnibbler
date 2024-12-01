VT Nibbler
==========

![Screenshot](screenshot.png)

This is a clone of the 1980's [Nibbler] arcade game, designed to be played on
a DEC VT terminal. It requires at least a VT320 (or something of comparable
functionality), but a VT525 is best if you want color and sound effects.

You'll also need at least a 19200 baud connection to play at the default
frame rate. If you find the input is lagging, try selecting a slower speed
using the command line option `--speed 4` or `--speed 3`.

[Nibbler]: https://en.wikipedia.org/wiki/Nibbler_(video_game)


Controls
--------

Use the arrow keys to move, and `Q` to quit.


Download
--------

The latest binaries can be found on GitHub at the following url:

https://github.com/j4james/vtnibbler/releases/latest

For Linux download `vtnibbler`, and for Windows download `vtnibbler.exe`.


Build Instructions
------------------

If you want to build this yourself, you'll need [CMake] version 3.15 or later
and a C++ compiler supporting C++20 or later.

1. Download or clone the source:  
   `git clone https://github.com/j4james/vtnibbler.git`

2. Change into the build directory:  
   `cd vtnibbler/build`

3. Generate the build project:  
   `cmake -D CMAKE_BUILD_TYPE=Release ..`

4. Start the build:  
   `cmake --build . --config Release`

[CMake]: https://cmake.org/


Supported Terminals
-------------------

| Terminal           | Color | Sound |
|--------------------|:-----:|:-----:|
| DEC VT320          | no    | no    |
| DEC VT330/340      | no    | no    |
| DEC VT382          | no    | no    |
| DEC VT420          | no    | no    |
| DEC VT510/520      | no    | no    |
| DEC VT525          | yes   | yes   |
| KoalaTerm          | no    | no    |
| MLTerm             | part  | no    |
| PowerTerm          | no    | no    |
| Reflection Desktop | no    | no    |
| RLogin             | yes   | yes   |
| VTStar             | part  | no    |
| Windows Terminal   | yes   | yes   |

You could also get by with a VT220 or VT240, but those terminals don't have
full-cell fonts, so the graphics will look a bit messed up.

Terminals with *part* color support will render the graphics in color, but
won't show palette animations and the different level color schemes.


License
-------

The VT Nibbler source code and binaries are released under the MIT License.
See the [LICENSE] file for full license details.

[LICENSE]: LICENSE.txt
