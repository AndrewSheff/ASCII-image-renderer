# PPM to ASCII Renderer

<p align="center">
  <img src="https://img.shields.io/badge/language-C-blue" alt="C"/>
  <img src="https://img.shields.io/badge/dependencies-none-brightgreen" alt="No dependencies"/>
  <img src="https://img.shields.io/badge/license-MIT-yellow" alt="MIT License"/>
</p>

Lightweight terminal utility that renders `.ppm` images directly in your terminal using **24-bit RGB ANSI escape sequences**. No third-party libraries — just pure C and your terminal's color capabilities.

Each pixel of the source image maps to a colored space character via:
```c
printf("\e[48;2;%d;%d;%dm ", r, g, b);
```

## Features

- **PPM (P6) rendering** — reads binary PPM files and displays them in the terminal
- **Interpolation** — optional pixel smoothing for better quality at small sizes
- **Aspect ratio preservation** — automatically corrects for terminal character proportions (2:1)
- **Auto-sizing** — detects terminal dimensions when no size is specified
- **Two modes** — interactive file picker or CLI with flags
- **Zero dependencies** — only standard C libraries

## Examples

| Without interpolation | With interpolation |
| --- | --- |
| <img width="307" height="426" alt="without_inter" src="https://github.com/user-attachments/assets/98a9932d-60d0-41e1-879e-e9164d2184a2" /> | <img width="307" height="426" alt="inter" src="https://github.com/user-attachments/assets/dc5e5cec-838c-4596-b619-04502bc5d4b7" /> |
| `./renderer test.ppm` | `./renderer test.ppm --inter` |

## Build

```bash
chmod +x build.sh
./build.sh
```

Or manually:
```bash
gcc -o renderer -I./include main.c terminal.c
```

## Usage

### Interactive mode

```bash
./renderer
```

The program will prompt you to:
1. Select a `.ppm` file from the current directory
2. Set output width and height (in characters)
3. Choose whether to preserve original proportions
4. Enable or disable interpolation

### CLI mode

```bash
./renderer <file.ppm> [options]
```

| Option | Description |
| --- | --- |
| `<file.ppm>` | Path to a PPM file (P6 binary format) |
| `--width N` | Output width in characters |
| `--height N` | Output height in characters |
| `--no-proportions` | Disable aspect ratio preservation |
| `--inter` | Enable interpolation (smoothing) |

**Examples:**
```bash
./renderer avatar.ppm                              # auto-size to terminal
./renderer test.ppm --inter                         # with smoothing
./renderer test.ppm --width 120 --height 40         # custom size
./renderer test.ppm --width 80 --no-proportions     # stretch to width
```

## How it works

```
┌─────────────┐     ┌──────────────┐     ┌───────────────┐     ┌──────────────┐
│  Read PPM   │────>│   Scale to   │────>│  Interpolate  │────>│  Print ANSI  │
│  P6 header  │     │ terminal size│     │  (optional)   │     │  RGB colors  │
│  + pixels   │     │              │     │               │     │              │
└─────────────┘     └──────────────┘     └───────────────┘     └──────────────┘
```

1. **Parse** — reads PPM P6 header (format, dimensions, max brightness) and raw RGB pixel data
2. **Scale** — computes dividers to fit the image into the target character grid, adjusting for the 2:1 terminal character aspect ratio
3. **Interpolate** — when `--inter` is enabled, averages neighboring pixel colors to reduce aliasing
4. **Render** — outputs colored space characters using `\e[48;2;R;G;Bm` (background color)

## Project structure

```
.
├── main.c            # Core logic: PPM parsing, scaling, interpolation, rendering
├── terminal.c        # Terminal utilities: raw mode, key input, screen clear
├── include/
│   └── terminal.h    # Terminal header with ANSI color constants
├── build.sh          # Build script
└── LICENSE           # MIT License
```

## Requirements

- **GCC** (or any C99-compatible compiler)
- Terminal with **24-bit True Color** support
  - Linux: most modern terminals (GNOME Terminal, Alacritty, Kitty, WezTerm)
  - macOS: iTerm2 (recommended), Terminal.app (macOS Sonoma+)
  - Windows: WSL + Windows Terminal

## Converting images to PPM

You can use ImageMagick or ffmpeg to convert any image to PPM:

```bash
# ImageMagick
convert input.png output.ppm

# ffmpeg
ffmpeg -i input.jpg output.ppm
```

## License

MIT © 2026 Andrew Shevtsov
