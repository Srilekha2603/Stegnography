/*Name : S.Srilekha
Date : 23-01-2026
Description :LSB-based image steganography to securely hide and extract 
             secret data within a BMP image without visible distortion.*/
#include <stdio.h>
#include <string.h>
#include "decode.h"

Status do_decoding(char *argv[],DecodeInfo *decInfo)
{
    printf("## Decoding procedure Started ##\n");
    /*Validate command-line arguments for decoding*/
    if(validate_decode_args(argv,decInfo) == failure)
        return failure;
    /*Decode and verify the magic string*/
    if(decode_magic_string(MAGIC_STRING,decInfo)==failure)
        return failure;
    /*Decode secret file extension and create output file*/
    if(decode_secret_file_extn(decInfo) == failure)
        return failure;
    /*Decode actual secret file data*/
    if(decode_secret_file_data(decInfo)== failure)
        return failure;
    return success;
}
Status validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    if(argv[3] == NULL)
    {
        strcpy(decInfo->output_fname,"secretfile");
        printf("Output file not mentioned. Creating %s as default\n",decInfo->output_fname);
    }
    else if(argv[3] != NULL)
    {
        int i=0;
        if(strstr(argv[3],"."))
        {
            for(i=0;i<strlen(argv[3]);i++)
            {
                if(argv[3][i]=='.')
                    break;
                decInfo->output_fname[i]=argv[3][i];
            }
            decInfo->output_fname[i]='\0';
        }
    }
    if(argv[2])
    {
        char * res = strstr(argv[2],".bmp");
        if((res == NULL) || (strcmp(res,".bmp") != 0))
            return failure;
        else 
        {
            decInfo->encoded_image_fname=argv[2];
            printf("It's a %s file\n",decInfo->encoded_image_fname);
        }
    }
    printf("Opening required files\n");
    if(open_image_files(decInfo)==failure)
        return failure;
    return success; 
}
Status open_image_files(DecodeInfo *decInfo)
{
    /*Open encoded image file in read mode*/
    decInfo->encode_image_fptr = fopen(decInfo->encoded_image_fname, "r");
    printf("Opened %s\n",decInfo->encoded_image_fname);
    if (decInfo->encode_image_fptr == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->encoded_image_fname);

    	return failure;
    }
    return success;
}
Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo)
{
    /*Skip BMP header 54 bytes*/
    fseek(decInfo->encode_image_fptr,54,SEEK_SET);
    /*Read 32 bytes to decode magic string length*/
    char buffer[32];
    fread(buffer,1,32,decInfo->encode_image_fptr);
    int magic_len=decode_4byte_from_lsb(buffer);
    if(magic_len < 0)
    {
        printf("Magic String is not decoded successfully\n");
        return 0;
    }
    printf("Decoding Magic String Signature\n");
    /*Decode each character of magic string*/
    char magic_str[magic_len+1];
    char bufferstr[8];
    for(int i=0;i<magic_len;i++)
    {
        fread(bufferstr,1,8,decInfo->encode_image_fptr);
        magic_str[i]=decode_1byte_from_lsb(bufferstr);
    }
    magic_str[magic_len]='\0';
    // for(int j=0;j<magic_len;j++)
    //     printf("%c",magic_str[j]);
    // printf("\n");
    if(strcmp(magic_str,MAGIC_STRING)==0)                                        //Compare decoded magic string with expected one
    {
        printf("Done\n");
        return success;
    }
    return failure;
}
Status decode_secret_file_extn(DecodeInfo *decInfo)
{
    /*Read extension length*/
    printf("Decoding secret file extension from stego image \n");
    char buffer_len[32];
    fread(buffer_len,1,32,decInfo->encode_image_fptr);
    int ext_len = decode_4byte_from_lsb(buffer_len);
    /*Decode each character of extension*/
    char ext[ext_len+1];
    char buffer_ext[8];
    for(int i=0;i<ext_len;i++)
    {
        fread(buffer_ext,1,8,decInfo->encode_image_fptr);
        ext[i]=decode_1byte_from_lsb(buffer_ext);
    }
    ext[ext_len]='\0';
    // for (int i=0;i<ext_len;i++)
    //     printf("%c",ext[i]);
    // printf("\n");
    strcat(decInfo->output_fname,ext);      //Append extension to output filename
    /*Opening the file to write decoded data*/
    decInfo->output_fptr = fopen(decInfo->output_fname,"w");
    if(decInfo->output_fptr == NULL)
        return failure;
    printf("Opened %s\n",decInfo->output_fname);
    printf("Done. Opened all required files\n");
    return success;
}
Status decode_secret_file_data(DecodeInfo *decInfo)
{
    /*Read secret file size*/
    printf("Decoding file size\n");
    char buffer[32];
    fread(buffer,1,32,decInfo->encode_image_fptr);
    int file_size = decode_4byte_from_lsb(buffer);
    printf("Done\n");
    /*Decode each character of secret data*/
    char bufferstr[8];
    char ch;
    printf("Decoding file data\n");
    for(int i=0;i<file_size;i++)
    {
        fread(bufferstr,1,8,decInfo->encode_image_fptr);
        ch = decode_1byte_from_lsb(bufferstr);
        fwrite(&ch,1,1,decInfo->output_fptr);
    }
    printf("Done\n");
    return success; 
}
/*Extract LSBs to reconstruct one character and Return decoded character*/
char decode_1byte_from_lsb(char *buffer_8)
{
    char ch =0;
    for(int i=7;i>=0;i--)
    {
        ch = ch | (buffer_8[7-i] & 1) << i;
    }
    return ch;
}
/*Extract LSBs to reconstruct integer value and return decoded integer*/
int decode_4byte_from_lsb(char *buffer_32)
{
    int val =0;
    for(int i=31;i>=0;i--)
    {
        val = val | (buffer_32[31-i] & 1) << i;
    }
    return val;
}