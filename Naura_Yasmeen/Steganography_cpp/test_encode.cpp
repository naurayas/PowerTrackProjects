/* Name         : Naura Yasmeen U
   Reg No.      : 25017_094
   Start Date   : 31/07/2026
   End Date     : 07/08/2026
   Project Name : Steganography
   Description  : This project implements a steganography system using .bmp image files in CPP.
                  It allows user to hide secret data inside the image by modifying the
                  LSB bits of the pixel values, generating a new encoded image that looks 
                  identical to the original. The program can also be used to decode the hidden
                  information by extracting the embedded bits. This ensures security of data
                  transmission without altering the visible quality of the image.
*/

#include "encode.h"
#include "decode.h"

OperationType checkOperationType(int argc, char *argv[]);
void printUsage();

int main(int argc, char *argv[])
{
   if (argc < 2)
   {
      std::cout << "Invalid option" << std::endl;
     printUsage();
     return 1;
   }

   switch(checkOperationType(argc, argv))
   {
      case e_encode:
      {
         std::cout << "Selected Encoding" << std::endl;
         try
         {
            Encoder encoder(argc, argv);

            std::cout << "Read and validate input arguments is successful" << std::endl;

            if(encoder.encode())
            {
               std::cout << "Completed encoding" << std::endl;
            }
            else
            {
               std::cout << "Failed to do encoding" << std::endl;
            }
         }
         catch(const std::exception &)
         {
            std::cout << "Failed to validate the input arguments" << std::endl;
         }
         break;
      }
      case e_decode:
      {
         std::cout << "Selected Decoding" << std::endl;

         try
         {
               Decoder decoder(argc, argv);

               std::cout << "Read and validate input arguments is successful" << std::endl;

               if (decoder.decode())
               {
                  std::cout << "Completed decoding" << std::endl;
               }
               else
               {
                  std::cout << "Failed to do the decoding" << std::endl;
               }
         }
         catch (const std::exception &)
         {
               std::cout << "Failed to validate the input arguments" << std::endl;
         }

         break;
      }
      default:
      {
         std::cout << "Invalid option" << std::endl;
         printUsage();
      }
   }
   return 0;
}

OperationType checkOperationType(int argc, char *argv[])
{
    if (argc < 2)
        return e_unsupported;

    if (strcmp(argv[1], "-e") == 0)
        return e_encode;

    if (strcmp(argv[1], "-d") == 0)
        return e_decode;

    return e_unsupported;
}

void printUsage()
{
    std::cout << "--------------------------Usage-------------------------\n";
    std::cout << "Encoding: ./stego -e beautiful.bmp secret.txt stego.bmp\n";
    std::cout << "Decoding: ./stego -d stego.bmp decoded.txt\n";
}