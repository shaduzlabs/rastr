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
PNG and TrueType conversion utilities are available in the [main repository](https://www.github.com/shaduzlabs/rastr)