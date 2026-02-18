
![logo](example/slim_logo.png)

# SLIM
**SLIM (SLeptsov IMage)** – An image encoding and compression format developed as a replacement for the DDS format. This format is designed for storing raster graphics and supports resolutions up to 65535x65535 pixels. It uses lossless compression algorithms (**RLE**, **RICE**, **SLDD**, **MASKARED**) and also employs a smart quantization algorithm.

# ToSLIM
**ToSLIM** – This is a tool for basic work with the **SLIM** format. This tool allows you to convert, compare, and view **SLIM** format files, and also supports other formats such as **PNG**, **JPG/JPEG**, **GIF**, **BMP**, **PIC** (supported by **stb_image.h**).

## Implemented
**Attention:** this project uses a cut version of **SLIM (miniSLIM)**. Currently, **miniSLIM** implements the following features.
#### Implement:
- ✅ Lossy compression
- ✅ Lossless compression
- ✅ Alpha channel and blending modes
- ✅ Support for 4K and 8K resolution images
- ❌ MIP mapping support
- ❌ Multilayer support

## Usage

| Flag | Description                                      | Notes                                |
|:----:|:-------------------------------------------------|:-------------------------------------|
| `-i`   | Show detailed information about an image       | format, size, bit depth, etc.        |
| `-c`   | Convert image from one format to another       | requires two file paths              |
| `-q N` | Set quality level for JPEG / SLIM output (0–255) | 255 = lossless / maximum quality   |
| `-v`   | Display the image (default behavior if no other action) | can be omitted              |
| `-a`   | Compare two images using PSNR and SSIM metrics | requires two files                   |
| `-h`   | Show this help message                         |                                      |
| `-y`   | Overwrite output file without asking           |                                      |

## Examples
| Command                                    | Description                               |
|:-------------------------------------------|:------------------------------------------|
| `toslim image.png`                         | Show the image                            |
| `toslim -v image.SLIM`                     | Show image (flag explicitly specified)    |
| `toslim -c image.png image.SLIM`           | Convert PNG → SLIM                        |
| `toslim -c image.SLIM image.png`           | Convert SLIM → PNG                        |
| `toslim -c -q 128 image.SLIM image.png`    | Convert with specified quality (~50%)     |
| `toslim -a image.SLIM image.png`           | Compare two images ( PSNR / SSIM / PSQNR )|


## Installation
Attention, you may need to compile SDL2 manually.

**Default build (graphical mode)**
`make`

**Terminal mode only build**
`make terminal`

## External dependencies

| Name       | URL                                          | Commit/Tag                          |
|------------|----------------------------------------------|-------------------------------------|
| SDL2       | https://github.com/libsdl-org/SDL            | cf5dabd6 (release-2.32.0 + patches) |
| stb        | https://github.com/nothings/stb              | master                              |

## License

This project is licensed under the [MIT License](LICENSE).

Copyright (c) 2026 Sleptsov Vladimir