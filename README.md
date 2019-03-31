```
,-.  ,-.  ,-.  |-  ,-.
|    ,-|  `-.  |   |
'    `-^  `-'  `'  '
```

`rastr` is a lightweight graphics library written in c++.

### Features
- No dependency to other software libraries (not even the `libstd`)
- No dynamic memory allocation (except for `DynamicCanvas`)
- Basic drawing primitives (lines, triangles, rectangles, circles) and bitmap images/fonts support
- Only integer arithmetic: can be used even on small embedded devices (tested on Arduino, Teensy, ESP32 and many other platforms)
- Comes with a set of tools (see below) to convert fonts and images to a format that is usable by the library
- Header only

### Tools
The tools below can be used to convert, respectively, png images and TrueType fonts to header files that can be used by `rastr`.
Please be aware that, in contrast to the library itself, the tools have some dependencies that need to be installed when building (`boost`, `libfreetype` and `libpng`).

#### Png converter
Images in `png` format can be converted to a header file using `rastr-png`. The library only supports 1bpp (monochrome) and 8bpp (grayscale) images. For the full list of parameters, please run the utility with `--help`.

#### Font converter
Fonts in TrueType format can be converted to a header file using `rastr-ttf`. Characters are rendered as a bitmaps (either 1bpp or 8bpp). For the full list of parameters, please run the utility with `--help`.
