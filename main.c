/*Name : S.Srilekha
Date : 23-01-2026
Description :LSB-based image steganography to securely hide and extract 
             secret data within a BMP image without visible distortion.*/
#include <stdio.h>
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encodeInfo;
    DecodeInfo decInfo;
    if( argc == 1 )
    {
        printf("Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
        printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
	    return 0;
    }

    int opr = check_operation(argv[1]);

    if( opr == encode )
    {
        if(argc < 4 || argc > 5)
        {
            printf("Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
            printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
            return 0;
        }
        if (do_encoding(argv,&encodeInfo) == failure)
        {
            printf("read and validate failed\n");
            return 0;
        }
        else
            printf("## Encoding Done Successfully ##\n");  
    }
    else if( opr == decode )
    {
        if(argc < 3  || argc > 4)
        {
            printf("Encoding: ./a.out -e <.bmp file> <.txt file> [output file]\n");
            printf("Decoding: ./a.out -d <.bmp file> [output file]\n");
            return 0;
        }
        if(do_decoding(argv,&decInfo) == failure)
        {
            printf("Decoding FAILED\n");
            return 0;
        }
        else
            printf("## Decoding Done Successfully ##\n"); 
    }
    else
    {
        printf("Invalid option.\n");
        printf("Please pass either -e for encoding or -d for decoding.\n");// Print -> Error + usage msg
    }
    return 0;
}