#include "decode.h"

Decoder::Decoder(int argc, char *argv[])
{
    if(argc < 3)
        throw std::invalid_argument("Invalid arguments");
    
    if(argv[2] != nullptr && (strcmp(strstr(argv[2], "."), ".bmp") != 0))
        throw std::invalid_argument("Invalid stego image");

    stegoImageFname = argv[2];

    if(argc >= 4)
        outputFileFname = argv[3];
    else
        outputFileFname = "decoded.txt";
}

bool Decoder::openFiles()
{
    stegoImage.open(stegoImageFname, std::ios::binary);

    if (!stegoImage.is_open())
    {
        std::cerr << "ERROR: Unable to open file " << stegoImageFname << std::endl;
        return false;
    }

    outputFile.open(outputFileFname, std::ios::binary);

    if (!outputFile.is_open())
    {
        std::cerr << "ERROR: Unable to open file " << outputFileFname << std::endl;
        closeDecodeFiles();
        return false;
    }

    return true;
}

void Decoder::closeDecodeFiles()
{
    stegoImage.close();
    outputFile.close();
}

void Decoder::skipBmpHeader()
{
    stegoImage.seekg(54, std::ios::beg);
}

bool Decoder::decodeMagicString()
{
    int len = strlen(MAGIC_STRING);
    char magic[len + 1];

    for (int i = 0; i < len; i++)
    {
        // Read 8 bytes from stego image
        if (!stegoImage.read(imageData, 8))
            return false;

        // Decode one character
        magic[i] = decodeByteFromLSB(imageData);
    }

    magic[len] = '\0';

    if (strcmp(magic, MAGIC_STRING) == 0)
        return true;

    return false;
}

unsigned int Decoder::decodeSizeFromLSB(char *imageBuffer)
{
    unsigned int ch = 0;

    for (int i = 0; i < 32; i++)
    {
        ch = (ch << 1) | (imageBuffer[i] & 1);
    }

    return ch;
}

bool Decoder::decodeSecretFileExtnSize()
{
    char rgb[32];

    // Read 32 bytes from the stego image
    if (!stegoImage.read(rgb, 32))
        return false;

    // Decode extension size
    extSize = decodeSizeFromLSB(rgb);

    return true;
}

char Decoder::decodeByteFromLSB(char *imageBuffer)
{
    unsigned char ch = 0;

    for (int i = 0; i < 8; i++)
    {
        ch = (ch << 1) | (imageBuffer[i] & 1);
    }

    return ch;
}

bool Decoder::decodeSecretFileExtn()
{
    for (int i = 0; i < extSize; i++)
    {
        // Read 8 bytes from stego image
        if (!stegoImage.read(imageData, 8))
            return false;

        // Decode one character
        extData[i] = decodeByteFromLSB(imageData);
    }

    extData[extSize] = '\0';
    return true;
}

bool Decoder::decodeSecretFileSize()
{
    char rgb[32];
    if(!stegoImage.read(rgb, 32))
    {
        return false;
    }
    secretFileSize = decodeSizeFromLSB(rgb);
    return true;
}

bool Decoder::decodeSecretFileData()
{
    char ch;

    for (int i = 0; i < secretFileSize; i++)
    {
        // Read 8 bytes from stego image
        if (!stegoImage.read(imageData, 8))
            return false;

        // Decode one character
        ch = decodeByteFromLSB(imageData);

        // Write decoded character to output file
        if (!outputFile.write(&ch, 1))
            return false;
    }

    return true;
}

bool Decoder::decode()
{
    if(!openFiles())
    {
        std::cout << "Failed tp open files" << std::endl;
        closeDecodeFiles();
        return false;
    }

    std::cout << "Opened all files successfully" << std::endl;

    // skip bmp header
    skipBmpHeader();

    if(!decodeMagicString())
    {
        std::cout << "Failed to decode magic string" << std::endl;
        closeDecodeFiles();
        return false;
    }

    std::cout << "Sucessfully decoded magic string" << std::endl;

    if(!decodeSecretFileExtnSize())
    {
        std::cout << "Failed to decode secret file extension size" << std::endl;
        closeDecodeFiles();
        return false;
    }

    // validate extn
    if(extSize <= 0)
    {
        std::cout << "Failed to decode secret file extension size" << std::endl;
        closeDecodeFiles();
        return false;
    }

    std::cout << "Successfully decoded secret file extension size" << std::endl;
    std::cout << "Decoded secret file extension size = " << extSize << std::endl;

    if(!decodeSecretFileExtn())
    {
        std::cout << "Failed to decode secret file extension" << std::endl;
        closeDecodeFiles();
        return false;
    }

    std::cout << "Successfully decoded secret file extension" << std::endl;
    std::cout << "Secret file extension = " << extData << std::endl;

    if(!decodeSecretFileSize())
    {
        std::cout << "Failed to decode secret file size" << std::endl;
        closeDecodeFiles();
        return false;
    }

    std::cout << "Successfully decoded the secret file size" << std::endl;
    std::cout << "Secret file size = " << secretFileSize << std::endl;

    if(!decodeSecretFileData())
    {
        std::cout << "Failed to decode secret file data" << std::endl;
        closeDecodeFiles();
        return false;
    }

    std::cout << "Successfully decoded secret file data" << std::endl;
    closeDecodeFiles();

    return true;
}