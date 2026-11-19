#include <iostream>    // For cout, cerr
#include <cstring>     // For strcmp, strlen
#include "decode.h"    // For decodeInfo struct + function declarations
#include "type.h"      // For Status, OperationType, uint
#include "common.h"    // For MAGIC_STRING
using namespace std;

Status read_validate_decode_args(char *argv[], DecodeInfo &decInfo)
{
    if (argv[2] == NULL)
    {
        cout << "bmp file has not been passed" << endl;
        return e_failure;
    }

    // check argv[2] is bmp file
    if (strstr(argv[2], ".bmp") == NULL)
    {
        cout << "Invalid bmp file" << endl;
        return e_failure;
    }

    // storing argv[2] stego file name to structure
    decInfo.stego_fname = argv[2];

    if (argv[3] == NULL)
    {
        decInfo.output_fname = "output"; // file name
    }
    else
    {
        // store user given output file name
        decInfo.output_fname = argv[3];
    }
    return e_success;
}

Status do_decoding(DecodeInfo &decInfo)
{
    cout << "-----------DECODED STARTED--------------" << endl;
    if (open_file(decInfo) == e_failure)
    {
        cout << "File opening failed" << endl;
        return e_failure;
    }
    cout << "Decoded file is opened successfully" << endl;

    if (decode_magic_string(MAGIC_STRING, decInfo) == e_failure)
    {
        cout << "decoded magic string is not copied" << endl;
        return e_failure;
    }
    cout << "After magic string, tellg = " << decInfo.fptr_stego.tellg() << endl;

    if (decode_extn_size(decInfo) == e_failure)
    {
        cout << "The decoded extn file size failed" << endl;
        return e_failure;
    }

    if (decode_extn(decInfo) == e_failure)
    {
        cout << "The decoded file extension is failed" << endl;
        return e_failure;
    }

    if (decode_file_size(decInfo) == e_failure)
    {
        cout << "The decoded file size is failed" << endl;
        return e_failure;
    }

    if (decode_file_data(decInfo) == e_failure)
    {
        cout << "The encoded secret data is not copied to decoded output file" << endl;
        return e_failure;
    }

    cout << "The data is decoded to output file successfully!!" << endl;
    return e_success;
}

// open files
Status open_file(DecodeInfo &decInfo)
{
    decInfo.fptr_stego.open(decInfo.stego_fname, ios::in | ios::binary);
    if (!decInfo.fptr_stego)
    {
        cout << "decoded file not opened" << endl;
        return e_failure;
    }
    return e_success;
}

// generic function - return int
int decode_lsb_to_size(char *image_buffer)
{
    int size = 0;
    int n = 31;
    for (int i = 0; i < 32; i++)
    {
        // get lsb
        int bit = image_buffer[i] & 1;
        // left shift bit n times
        bit = bit << n;
        // write bit variable to size
        size = size | bit;
        n--;
    }
    return size;
}

// generic function - return char
char decode_lsb_to_bytes(char *image_buffer)
{
    char ch = 0;
    int n = 7;
    for (int i = 0; i < 8; i++)
    {
        // get lsb
        char bit = image_buffer[i] & 1;
        // left shift bit n times
        bit = bit << n;
        // write bit variable to ch
        ch = ch | bit;
        n--;
    }
    return ch;
}

Status decode_magic_string(const char *magic_str, DecodeInfo &decInfo)
{
    // skip 54 bytes
    decInfo.fptr_stego.seekg(54, ios::beg);

    char magic_string[10];
    int i;

    for (i = 0; i < 2; i++)
    {
        char buffer[8];
        // read 8 bytes of data buffer from stego
        decInfo.fptr_stego.read(buffer, 8);
        if (decInfo.fptr_stego.gcount() != 8)
        {
            cout << "Error: decoded failed to read magic string" << endl;
            return e_failure;
        }
        magic_string[i] = decode_lsb_to_bytes(buffer);
    }
    magic_string[i] = '\0';

    char str[20];
    cout << "Enter your magic string: ";
    cin.getline(str, 20);

    if (strcmp(str, magic_str) == 0) // compare both - use passed magic_str
    {
        cout << "The magic string is correct, The person is authorized!!" << endl;
        return e_success;
    }
    else
    {
        cout << "The magic string is wrong" << endl;
        return e_failure;
    }
}

Status decode_extn_size(DecodeInfo &decInfo)
{
    char buffer[32];
    // read 32 bytes from stego file
    decInfo.fptr_stego.read(buffer, 32);
    if (decInfo.fptr_stego.gcount() != 32)
        return e_failure;

    decInfo.extn_size = decode_lsb_to_size(buffer);
    cout << "Decoded extension size: " << decInfo.extn_size << endl;
    return e_success;
}

Status decode_extn(DecodeInfo &decInfo)
{
    for (int i = 0; i < decInfo.extn_size; i++)
    {
        char buffer[8];
        // read 8 bytes from stego file and store to buffer
        decInfo.fptr_stego.read(buffer, 8);
        if (decInfo.fptr_stego.gcount() != 8)
            return e_failure;

        decInfo.extn[i] = decode_lsb_to_bytes(buffer);
    }
    decInfo.extn[decInfo.extn_size] = '\0'; // null terminator

    cout << "Decoded extension: " << decInfo.extn << endl;

    // merged output file name with decoded extn
    // If user gave "output" only, add extn. If user gave full name, keep it.
    if (decInfo.output_fname == "output")
    {
        decInfo.output_fname += decInfo.extn;
    }

    decInfo.fptr_output.open(decInfo.output_fname, ios::out | ios::binary);
    if (!decInfo.fptr_output)
    {
        cout << "output decoded file not opened" << endl;
        return e_failure;
    }
    cout << "Output file opened" << endl;
    return e_success;
}

Status decode_file_size(DecodeInfo &decInfo)
{
    char buffer[32];
    // read 32 bytes from stego to store buffer
    decInfo.fptr_stego.read(buffer, 32);
    if (decInfo.fptr_stego.gcount() != 32)
        return e_failure;

    decInfo.file_size = decode_lsb_to_size(buffer);
    cout << "Decode sec file size: " << decInfo.file_size << endl;
    return e_success;
}

Status decode_file_data(DecodeInfo &decInfo)
{
    char image_buffer[8];
    char ch;

    for (int i = 0; i < decInfo.file_size; i++)
    {
        decInfo.fptr_stego.read(image_buffer, 8);
        if (decInfo.fptr_stego.gcount() != 8)
            return e_failure;

        ch = decode_lsb_to_bytes(image_buffer);
        decInfo.fptr_output.put(ch);
    }

    decInfo.fptr_output.close();
    decInfo.fptr_stego.close();
    cout << "All the data are copied to output file successfully!!" << endl;
    return e_success;
}