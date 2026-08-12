#include "encode.h"

Encoder::Encoder(int argc, char *argv[])
{
    // validate arg count
    if (argc < 4)
        throw std::invalid_argument("Insufficient arguments");
    // validate source image
    if (strcmp(strstr(argv[2], "."), ".bmp") != 0)
        throw std::invalid_argument("Invalid Source image");
    
    // validate secret file
    if(strcmp(strstr(argv[3], "."), ".txt") != 0)
        throw std::invalid_argument("Invalid secret file");

    srcImageFname = argv[2];
    secretFname = argv[3];

    if(argc >= 5)
        stegoImageFname = argv[4];
    else
        stegoImageFname = "stego.bmp";
}

unsigned int  Encoder::getImageSize()
{
    unsigned int width, height;
    // move to 18th byte
    srcImage.seekg(18);

    srcImage.read(reinterpret_cast<char*>(&width), sizeof(width));
    std::cout << "Width = " << width << std::endl;

    srcImage.read(reinterpret_cast<char*>(&height), sizeof(height));
    std::cout << "Height = " << height << std::endl;

    return width * height * 3;
}

bool Encoder::openFiles()
{
    // Src Image file
    srcImage.open(srcImageFname);
    // Do Error handling
    if (!srcImage.is_open())
    {
        std::cerr << "ERROR: Unable to open file " << srcImageFname << std::endl;
        return false;
    }

    // Secret file
    secretFile.open(secretFname);
    // Do Error handling
    if (!secretFile.is_open())
    {
        std::cerr << "ERROR: Unable to open file " << secretFname << std::endl;
        closeEncodeFiles();
        return false;
    }

    // Stego Image file
    stegoImage.open(stegoImageFname);
    // Do Error handling
    if (!stegoImage.is_open())
    {
        std::cerr << "ERROR: Unable to open file " << stegoImageFname << std::endl;
        closeEncodeFiles();
        return false;
    }

    // No failure return e_success
    return true;
}

void Encoder::closeEncodeFiles()
{
    srcImage.close();

    secretFile.close();

    stegoImage.close();
}

unsigned int Encoder::getFileSize()
{
    // move to end
    secretFile.seekg(0, std::ios::end);
    std::cout << "Size: " << secretFile.tellg() << std::endl;
    // get size
    return secretFile.tellg();
}

bool Encoder::checkCapacity()
{
    imageCapacity = getImageSize();
    sizeSecretFile = getFileSize();
    if(imageCapacity > ((54 + 2 + 4 + 4 + 4 + sizeSecretFile) * 8))
        return true;
    return false;
}

bool Encoder::copyBmpHeader()
{
    char header[54];
    srcImage.seekg(0);
    srcImage.read(header, 54);
    stegoImage.write(header, 54);
    return true;
}

void Encoder::encodeByteToLSB(char data, char *imageBuffer)
{
    unsigned char mask = 1 << 7;
    for (int i = 0; i < 8; i++)
    {
        imageBuffer[i] = (imageBuffer[i] & 0xFE) | ((data & mask) >> (7 - i));
        mask = mask >> 1;
    }
}

bool Encoder::encodeDataToImage(const char *data, int size)
{
    for (int i = 0; i < size; i++)
    {
        // read 8 bytes from beautiful.bmp
        if(!srcImage.read(imageData, 8))
            return false;
        // call encodeByteToLSB to encode the data to lsb
        encodeByteToLSB(data[i], imageData);
        if(!stegoImage.write(imageData, 8))
            return false;
    }
    return true;
}

bool Encoder::encodeMagicString()
{
    // all the character encoding will call encodeDataToImage
    return encodeDataToImage(MAGIC_STRING, strlen(MAGIC_STRING));
}

void Encoder::encodeSizeToLSB(char *imageBuffer, int size)
{
    unsigned int mask = 1 << 31;
    for (int i = 0; i < 32; i++)
    {
        imageBuffer[i] = (imageBuffer[i] & 0xFE) | ((size & mask) >> (31 - i));
        mask = mask >> 1;
    }
}

bool Encoder::encodeSize(unsigned int size)
{
    char rgb[32];
    // read 32 bytes for the source image
    if(!srcImage.read(rgb, 32))
        return false;
    // encode_size_to_lsb to encode the integer into lsb of image data 
    encodeSizeToLSB(rgb, size);
    if(!stegoImage.write(rgb, 32))
        return false;
    return true;
}

bool Encoder::encodeSecretFileExtn()
{
    return encodeDataToImage(".txt", strlen(".txt"));
}

bool Encoder::encodeSecretFileSize(long int size)
{
    char rgb[32];
    // read 32 bytes from the source image
    if(!srcImage.read(rgb, 32))
        return false;
    // encode_size_to_lsb to encode the integer into lsb of image data 
    encodeSizeToLSB(rgb, size);
    if(!stegoImage.write(rgb, 32))
        return false;
    return true;
}

bool Encoder::encodeSecretFileData()
{
    char ch;
    for (int i = 0; i < sizeSecretFile; i++)
    {
        // read a character from secret file
        if(!secretFile.read(&ch, 1))
        {
           //std::cout << "Failed reading secret file\n";
            return false;
        }
        // read 8 bytes of rgb from image file
        if(!srcImage.read(imageData, 8))
        {
            //std::cout << "Failed reading source image\n";
            return false;
        }
        encodeByteToLSB(ch, imageData);
        if(!stegoImage.write(imageData, 8))
        {
            //std::cout << "Failed writing stego image\n";
            return false;
        }
    }
    return true;
}

bool Encoder::copyRemainingImgData()
{
    char ch;
    while(srcImage.read(&ch, 1))
    {
        if(!stegoImage.write(&ch, 1))
            return false;
    }
    return true;
}

bool Encoder::encode()
{
    if(!openFiles())
    {
        std::cout << "Failed to open file" << std::endl;
        closeEncodeFiles();
        return false;
    }

    std::cout << "Opened all files successfully" << std::endl;

    if(!checkCapacity())
    {
        std::cout << "Failure! Image does not have enough capacity" << std::endl;
        closeEncodeFiles();
        return false;
    }

    std::cout << "Image have enough capacity to encode" << std::endl;

    // reset file pointer
    secretFile.seekg(0, std::ios::beg);

    if(!copyBmpHeader())
    {
        std::cout << "Failed to copy header" << std::endl;
        closeEncodeFiles();
        return false;
    }

    std::cout << "Successfully copied the header" << std::endl;

    if(!encodeMagicString())
    {
        std::cout << "Failes to encode magic string" << std::endl;
        closeEncodeFiles();
        return false;
    }

    std::cout << "Successfullt encoded the magic string" << std::endl;

    if(!encodeSize(strlen(".txt")))
    {
        std::cout << "Failed to encode the secret files extension size" << std::endl;
        closeEncodeFiles();
        return false;
    }

    std::cout << "Successfully encded secret file extension size" << std::endl;

    if(!encodeSecretFileExtn())
    {
        std::cout << "Failed to encode the secret file extension" << std::endl;
        closeEncodeFiles();
        return false;
    }

    std::cout << "Successfully encoded secret file extension" << std::endl;

    if(!encodeSecretFileSize(sizeSecretFile))
    {
        std::cout << "Failed to encode the secret file size" << std::endl; 
        closeEncodeFiles();
        return false;  
    }

    std::cout << "Successfully encoded the secret file size" << std::endl;

    if(!encodeSecretFileData())
    {
        std::cout << "Failed to encode the secret data" << std::endl;
        closeEncodeFiles();
        return false;
    }

    std::cout << "Successfully encoded the secret data" << std::endl;

    if (!copyRemainingImgData())
    {
        std::cout << "Failed to copy remaining RGB data" << std::endl;
        closeEncodeFiles();
        return false;
    }

    std::cout << "Successfully copied the remaining RGB data" << std::endl;

    closeEncodeFiles();


    return true;
}