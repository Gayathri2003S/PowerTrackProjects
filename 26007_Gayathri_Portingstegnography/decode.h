#ifndef DECODE_H
#define DECODE_H

#include <iostream>
#include <fstream>
#include <string>
#include"type.h"
using namespace std;

struct DecodeInfo
{
    //stego file name
    string stego_fname;   //stego file
    fstream fptr_stego;  //opened


    //info
    int extn_size;
    char extn[10];  //".txt"
    int file_size;  //25
    char *file_data;
    

    //output file info
    string output_fname;  //"output.txt"
    fstream fptr_output;  //opened

};

/* Decoding function prototype */

/* Read and validate decode args from argv */
Status read_validate_decode_args(char *argv[], DecodeInfo &decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo &decInfo);

/* open files*/
Status open_file(DecodeInfo &decInfo);

/* Decode magic string */
Status decode_magic_string(const char *magic_str, DecodeInfo &decInfo);

/* Decode lsb to size */
int decode_lsb_to_size(char *image_buffer);

/* Decode lsb to bytes */
char decode_lsb_to_bytes(char *image_buffer);

/* Decode secret file extn size*/
Status decode_extn_size(DecodeInfo &decInfo);

/* Decode secret  file extn */
Status decode_extn(DecodeInfo &decInfo);

/* Decode secret file size */ 
Status decode_file_size(DecodeInfo &decInfo);

/* Decode secret file data */
Status decode_file_data(DecodeInfo &decInfo);


#endif
 