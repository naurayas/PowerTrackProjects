# LSB Image Steganography (C++)

This project implements Least Significant Bit (LSB) Image Steganography in C++. It hides a secret file inside a 24-bit BMP image without causing noticeable visual changes. The hidden file can later be extracted accurately using the decoding program.

The project demonstrates concepts such as binary file handling, bit manipulation, object-oriented programming, and image processing.

## Features
- Hide any text file inside a 24-bit BMP image
- Extract the hidden file from the encoded image
- Preserves the original image quality
- Supports custom output filenames
- Displays encoding and decoding progress
- Error handling for invalid files and insufficient image capacity


## Working
### Encoding
The encoder performs the following steps:

1. Opens the source BMP image.
2. Opens the secret file.
3. Checks whether the image has sufficient capacity.
4. Copies the BMP header.
5. Encodes:
   - Magic string
   - Secret file extension
   - Secret file size
   - Secret file data
6. Generates the encoded BMP image.

### Decoding
The decoder performs the following steps:

1. Opens the encoded BMP image.
2. Verifies the magic string.
3. Reads the secret file extension.
4. Reads the secret file size.
5. Extracts the hidden data.
6. Reconstructs the original secret file.

## Technologies Used
- C++
- Object-Oriented Programming
- File Handling
- Bit Manipulation

## Project Structure
```bash
.
├── a.out
├── beautiful.bmp
├── decode.cpp
├── decode.h
├── decode.txt
├── encode.cpp
├── encode.h
├── secret.txt
├── stego.bmp
└── test_encode.cpp

```

## Compilation
``bash
g++ *.cpp -o stego
```

## Usage
```bash
Encoding: ./stego -e beautiful.bmp secret.txt stego.bmp
Decoding: ./stego -d stego.bmp decoded.txt
```
