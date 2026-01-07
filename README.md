# StrokeOrder: a tool for displaying the stroke order of kanji right from the comfort of your own command prompt

![Stroke order demo](neko.gif)

## Installation
To compile and install the program, simply do the following on a linux machine with gcc installed.
```sh
make
sudo make install
```
You may also uninstall by doing the following
```sh
sudo make uninstall
```

## Usage
To view a stroke order animation of some kanji, simply run the `strokeorder` command as shown below.
```sh
strokeorder 猫
```

## Credits

This software uses the following third-party resources:

### NanoSVG (TinySVG)
- Author: Mikko Mononen
- License: zlib/libpng-style license
- Description: A simple C library for parsing and rendering SVG files.
- Source: [https://github.com/memononen/nanosvg](https://github.com/memononen/nanosvg)

License excerpt:
> Permission is granted to anyone to use this software for any purpose,
> including commercial applications, and to alter it and redistribute it
> freely, subject to the following restrictions:
> 1. The origin of this software must not be misrepresented; you must not
>    claim that you wrote the original software.
> 2. Altered source versions must be plainly marked as such, and must not
>    be misrepresented as being the original software.
> 3. This notice may not be removed or altered from any source distribution.

---

### KanjiVG
- Author: Ulrich Apel
- License: [Creative Commons Attribution-ShareAlike 3.0 (CC BY-SA 3.0)](https://creativecommons.org/licenses/by-sa/3.0/)
- Description: Vector graphics data for kanji stroke order.
- Source: [https://kanjivg.tagaini.net](https://kanjivg.tagaini.net)

In this software, the KanjiVG SVG files have been modified with renamed file names and decreased stroke widths.

License requirements:
- Give proper credit to the original author (Ulrich Apel).  
- Indicate if you modified the data.  
- Share derivative data under the same license if redistributed.


