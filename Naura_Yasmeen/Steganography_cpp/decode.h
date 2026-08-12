#ifndef DECODE_H
#define DECODE_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <stdexcept>

#define MAGIC_STRING    "#*"

class Decoder
{
private:
    /* Stego Image Info */
    std::string stegoImageFname;
    std::ifstream stegoImage;

    /* Output Secret File Info */
    std::string outputFileFname;
    std::ofstream outputFile;

    /* Buffers */
    char imageData[8];
    char extData[5];
    char secretData;

    /* Decoded Information */
    unsigned int secretFileSize;
    char magicString[3];
    char fileExtn[5];
    unsigned int extSize;

public:
    Decoder(int argc, char *argv[]);    // Constructor
    bool decode();

private:
    bool openFiles();
    void closeDecodeFiles();
    void skipBmpHeader();
    bool decodeMagicString();
    char decodeByteFromLSB(char *imageBuffer);
    unsigned int decodeSizeFromLSB(char *imageBuffer);
    bool decodeDataFromImage(char *buffer, int size);
    bool decodeSecretFileExtnSize();
    bool decodeSecretFileExtn();
    bool decodeSecretFileSize();
    bool decodeSecretFileData();
};

#endif