#ifndef ENCODE_H
#define ENCODE_H

#include <iostream>
#include <cstring>
#include <fstream>
#include <stdexcept>

#define MAGIC_STRING    "#*"

enum OperationType
{
    e_encode,
    e_decode,
    e_unsupported
};

class Encoder
{
private:
    /* Source Image info */
    std::string srcImageFname;
    std::ifstream srcImage;
    unsigned int imageCapacity;
    unsigned int bitsPerPixel;
    char imageData[8];

    /* Secret File Info */
    std::string secretFname;
    std::ifstream secretFile;
    long sizeSecretFile;

    /* Stego Image Info */
    std::string stegoImageFname;
    std::ofstream stegoImage;

public:
    Encoder(int argc, char *argv[]);    // constructor
    bool encode();

private:
    
    bool openFiles();
    void closeEncodeFiles();
    bool copyBmpHeader();
    unsigned int getFileSize();
    unsigned int getImageSize();
    bool checkCapacity();
    void encodeByteToLSB(char data, char *imageBuffer);
    void encodeSizeToLSB(char *imageBuffer, int size);
    bool encodeDataToImage(const char *data, int size);
    bool encodeMagicString();
    bool encodeSecretFileExtn();
    bool encodeSecretFileSize(long int size);
    bool encodeSize(unsigned int size);
    bool encodeSecretFileData();
    bool copyRemainingImgData();
};

#endif
