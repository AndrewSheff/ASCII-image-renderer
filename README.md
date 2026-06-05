# PPM to ASCII Renderer
This is simple util, to render `.ppm` images in your terminal. 
Using RGB colors by using terminal control character like this:
```c 
printf("\e[48;2;%d;%d;%dm ", colors[r][c].red, colors[r][c].green, colors[r][c].blue);
```

No third-party libraries are used.

# Build
You can build this with `build.sh`.
```
chmod +x build.sh
./build.sh
```

# Usage

Interactive mode

`
./renderer
`

CLI mode

`
./renderer <filename.ppm> [--width N] [--height N] [--no-proportions] [--inter]
`
| Argument	| Description |
| --- | --- |
|filename.ppm	|Path to PPM file (P6 binary format)|
|--width N| Width in ASCII char |
|--height N| Height in ASCII char |
|--no-proportions| Off original proportions of image |
|--inter|	Enable interpolation (smoothing) |

# Examples

| Without interpolation | With interpolation |
| --- | --- |
| <img width="307" height="426" alt="without_inter" src="https://github.com/user-attachments/assets/98a9932d-60d0-41e1-879e-e9164d2184a2" /> | <img width="307" height="426" alt="inter" src="https://github.com/user-attachments/assets/dc5e5cec-838c-4596-b619-04502bc5d4b7" /> |
|./renderer test.ppm|./renderer test.ppm --inter|

# Requirements

Terminal with 24-bit color support (I use iTerm2 — great btw)

macOS / Linux (Windows WSL should work)

## License

MIT © 2026 Andrew Shevtsov
