#ifndef ENCODE_H
#define ENCODE_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdio>
#include "common.h"
#include "type.h" // Contains user defined types
using namespace std;

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

struct EncodeInfo
{
    /* Source Image info */
    string src_image_fname;  //beautiful.bmp
    fstream fptr_src_image;   //opened
    uint image_capacity = 0;
    uint bits_per_pixel = 0;
    vector<char> image_data;

    /* Secret File Info */
    string secret_fname;     //secret.txt
    fstream fptr_secret;
    string extn_secret_file; //.txt
    vector<char> secret_data;
    long size_secret_file = 0;    //25
    int extn_size = 0;          // 4

    /* Stego Image Info */
    string stego_image_fname;  //stego.bmp
    fstream fptr_stego_image;  //opened

} ;

/* Encoding function prototype */

/* Check operation type */
OperationType check_operation_type(char *argv[]);

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo &encInfo);

/* Perform the encoding */
Status do_encoding(EncodeInfo &encInfo);

/* Get File pointers for i/p and o/p files */
Status open_files(EncodeInfo &encInfo);

/* check capacity */
Status check_capacity(EncodeInfo &encInfo);

/* Get image size */
uint get_image_size_for_bmp(fstream &fptr_image);

/* Get file size */
uint get_file_size(fstream &fptr);

/* Copy bmp image header */
Status copy_bmp_header(fstream &fptr_src_image, fstream &fptr_dest_image);

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo &encInfo);

/* Encode secret  file extn size */
Status encode_extn_file_size(int extn_size, EncodeInfo &encInfo);

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo &encInfo);

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo &encInfo);

/* Encode function, which does the real encoding */
Status encode_data_to_image(char *data, int size, fstream &fptr_src_image, fstream &fptr_stego_image);

/* Encode a byte into LSB of image data array */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Encode a size into LSB of image data array */
Status encode_size_to_lsb(int data, char *image_buffer);

/* Encode secret file data */
Status encode_secret_file_data(EncodeInfo &encInfo);

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(fstream &fptr_src, fstream &fptr_dest);

#endif