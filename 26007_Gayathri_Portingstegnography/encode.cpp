#include "encode.h"   // For EncodeInfo struct + function declarations
#include "type.h"     // For Status, OperationType, uint
#include "common.h"    // For MAGIC_STRING
#include <iostream>    // For cout, cerr
#include <string>      // For string
#include <fstream>     // For fstream
#include <vector>      // For vector
#include <cstring>     // For strcmp, strlen
#include <cstdio>      // For FILE, fread, fwrite
using namespace std;

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(fstream &fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fptr_image.seekg(18, ios::beg);

    // Read the width (an int)
    fptr_image.read((char*)&width, sizeof(int));
    cout << "width = " << width << endl;

    // Read the height (an int)
    fptr_image.read((char*)&height, sizeof(int));
    cout << "height = " << height << endl;

    // Return image capacity
    return width * height * 3;
}

/* Get file size
 * Input: File ptr
 * Output: Size of file in bytes
 */
uint get_file_size(fstream &fptr)
{
    // Go to end
    fptr.seekg(0, ios::end);
    // Get position = size
    uint size = fptr.tellg();
    // Come back to start
    fptr.seekg(0, ios::beg);
    return size;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo &encInfo)
{
    // Src Image file
    encInfo.fptr_src_image.open(encInfo.src_image_fname, ios::in | ios::binary);
    // Do Error handling
    if (!encInfo.fptr_src_image)
    {
        cerr << "ERROR: Unable to open file " << encInfo.src_image_fname << endl;
        return e_failure;
    }

    // Secret file
    encInfo.fptr_secret.open(encInfo.secret_fname, ios::in | ios::binary);
    // Do Error handling
    if (!encInfo.fptr_secret)
    {
        cerr << "ERROR: Unable to open file " << encInfo.secret_fname << endl;
        return e_failure;
    }

    // Stego Image file
    encInfo.fptr_stego_image.open(encInfo.stego_image_fname, ios::out | ios::binary);
    // Do Error handling
    if (!encInfo.fptr_stego_image)
    {
        cerr << "ERROR: Unable to open file " << encInfo.stego_image_fname << endl;
        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// checking operation
OperationType check_operation_type(char *argv[])
{
    if (argv[1] == NULL)
        return e_unsupported;

    if (strcmp(argv[1], "-e") == 0) // encode
        return e_encode;

    if (strcmp(argv[1], "-d") == 0) // decode
        return e_decode;

    return e_unsupported;
}

// read and validate
Status read_and_validate_encode_args(char *argv[], EncodeInfo &encInfo)
{
    if (argv[2] == NULL)
    {
        cout << "bmp file has not been passed" << endl;
        return e_failure;
    }
    // check argv[2] is bmp file
    if (string(argv[2]).find(".bmp") == string::npos)
    {
        cout << "Invalid bmp file" << endl;
        return e_failure;
    }
    // storing argv[2] bmp file name to structure
    encInfo.src_image_fname = argv[2];

    if (argv[3] == NULL)
    {
        cout << "secret file is not present" << endl;
        return e_failure;
    }
    if (string(argv[3]).find('.') == string::npos)
    {
        cout << "Invalid secret file" << endl;
        return e_failure;
    }
    // store argv[3] to struct
    encInfo.secret_fname = argv[3];

    if (argv[4] == NULL)
    {
        encInfo.stego_image_fname = "stego.bmp"; // output file name
    }
    else
    {
        if (string(argv[4]).find(".bmp") == string::npos)
        {
            cout << "Invalid output file name" << endl;
            return e_failure;
        }
        encInfo.stego_image_fname = argv[4];
    }
    return e_success;
}

Status do_encoding(EncodeInfo &encInfo)
{
    // open files
    if (open_files(encInfo) == e_failure)
    {
        cout << "open files failed";
        return e_failure;
    }

    if (check_capacity(encInfo) == e_failure)
    {
        cout << "check capacity is failed" << endl;
        return e_failure;
    }

    // come back to start
    encInfo.fptr_src_image.seekg(0, ios::beg);

    if (copy_bmp_header(encInfo.fptr_src_image, encInfo.fptr_stego_image) == e_failure)
    {
        cout << "bmp copy header is failed" << endl;
        return e_failure;
    }

    cout << "[debug] header copy offset: " << encInfo.fptr_src_image.tellg() << endl;

    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        cout << "magic string is failed" << endl;
        return e_failure;
    }

    // display secret file size
    cout << "INFO: secret file size = " << encInfo.size_secret_file << " bytes" << endl;

    cout << "[debug] encode copy offset: " << encInfo.fptr_src_image.tellg() << endl;

    if (encode_extn_file_size(encInfo.extn_size, encInfo) == e_failure)
    {
        cout << "extn file size is failed" << endl;
        return e_failure;
    }

    // display extension size
    cout << "[ENCODE] encoded extension size: " << encInfo.extn_size << endl;

    cout << "[debug] encode copy offset: " << encInfo.fptr_src_image.tellg() << endl;

    if (encode_secret_file_extn(encInfo.extn_secret_file.c_str(), encInfo) == e_failure)
    {
        cout << "secret file extn is failed" << endl;
        return e_failure;
    }

    cout << "[debug] encode copy offset: " << encInfo.fptr_src_image.tellg() << endl;

    if (encode_secret_file_size(encInfo.size_secret_file, encInfo) == e_failure)
    {
        cout << "secret file size is failed" << endl;
        return e_failure;
    }

    cout << "[debug] encode copy offset: " << encInfo.fptr_src_image.tellg() << endl;

    // Display sec file data
    cout << "INFO: Encoding Secret file data" << endl;

    if (encode_secret_file_data(encInfo) == e_failure)
    {
        cout << "secret data is failed" << endl;
        return e_failure;
    }

    cout << "[debug] encode copy offset: " << encInfo.fptr_src_image.tellg() << endl;

    // Display secret data
    cout << "INFO: secret data encoded successfully" << endl;

    if (copy_remaining_img_data(encInfo.fptr_src_image, encInfo.fptr_stego_image) == e_failure)
    {
        cout << "copy remaining data is failed" << endl;
        return e_failure;
    }

    cout << "[debug] encode copy offset: " << encInfo.fptr_src_image.tellg() << endl;

    // encode to stego_image_fname
    cout << "INFO: Done" << endl;
    cout << "SUCCESS: Encoded " << encInfo.size_secret_file << " bytes of secret data into " << encInfo.stego_image_fname << endl;

    // close all files
    encInfo.fptr_src_image.close();
    encInfo.fptr_stego_image.close();
    encInfo.fptr_secret.close();

    return e_success;
}

Status check_capacity(EncodeInfo &encInfo)
{
    // get sec file extn
    size_t dotPos = encInfo.secret_fname.find('.');
    if (dotPos != string::npos)
    {
        encInfo.extn_secret_file = encInfo.secret_fname.substr(dotPos);
    }

    // get length of extn
    encInfo.extn_size = encInfo.extn_secret_file.length();

    // get sec file size
    encInfo.size_secret_file = get_file_size(encInfo.fptr_secret);

    // reset secret file back to first byte - get_file_size already does it
    encInfo.fptr_secret.seekg(0, ios::beg);

    int total_bytes_needed = 54 + 16 + 32 + (encInfo.extn_size * 8) + 32 + (encInfo.size_secret_file * 8);

    int bmp_file_size = get_image_size_for_bmp(encInfo.fptr_src_image);
    encInfo.fptr_src_image.seekg(0, ios::beg);

    if (total_bytes_needed <= bmp_file_size)
        return e_success;
    else
        return e_failure;
}

Status copy_bmp_header(fstream &fptr_src_image, fstream &fptr_dest_image)
{
    vector<char> buffer(54);

    // read 54 bytes from fptr_src_image
    fptr_src_image.read(buffer.data(), 54);
    if (fptr_src_image.gcount() != 54)
        return e_failure;

    // write 54 bytes to fptr_dest_image
    fptr_dest_image.write(buffer.data(), 54);
    if (!fptr_dest_image)
        return e_failure;

    return e_success;
}

// generic function
Status encode_size_to_lsb(int data, char *image_buffer)
{
    // image buffer size is 32 bytes
    int n = 31;
    for (int i = 0; i < 32; i++)
    {
        int bit = (data >> n) & 1;

        image_buffer[i] = image_buffer[i] & 0xfe;
        image_buffer[i] = image_buffer[i] | bit;
        n--;
    }
    return e_success;
}

Status encode_byte_to_lsb(char data, char *image_buffer)
{
    // image buffer arr size is 8 bytes
    int n = 7;
    for (int i = 0; i < 8; i++)
    {
        char bit = (data >> n) & 1;

        image_buffer[i] = image_buffer[i] & 0xfe;
        image_buffer[i] = image_buffer[i] | bit;
        n--;
    }
    return e_success;
}

Status encode_magic_string(const char *magic_string, EncodeInfo &encInfo)
{
    for (int i = 0; i < 2; i++)
    {
        vector<char> buffer(8);
        // read 8 bytes of data buffer from src

        encInfo.fptr_src_image.read(buffer.data(), 8);
        if (encInfo.fptr_src_image.gcount() != 8)
            return e_failure;

        encode_byte_to_lsb(magic_string[i], buffer.data());

        // write 8 bytes buffer to stego file
        encInfo.fptr_stego_image.write(buffer.data(), 8);
        if (!encInfo.fptr_stego_image)
            return e_failure;
    }
    return e_success;
}

Status encode_extn_file_size(int extn_size, EncodeInfo &encInfo)
{
    vector<char> buffer(32);

    // read 32 bytes from src file
    encInfo.fptr_src_image.read(buffer.data(), 32);
    if (encInfo.fptr_src_image.gcount() != 32)
        return e_failure;

    encode_size_to_lsb(extn_size, buffer.data());

    // write 32 bytes buffer to stego file
    encInfo.fptr_stego_image.write(buffer.data(), 32);
    if (!encInfo.fptr_stego_image)
        return e_failure;

    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo &encInfo)
{
    for (int i = 0; file_extn[i] != '\0'; i++)
    {
        vector<char> buffer(8);

        // read 8 bytes from src file and store to buffer
        encInfo.fptr_src_image.read(buffer.data(), 8);
        if (encInfo.fptr_src_image.gcount() != 8)
            return e_failure;

        encode_byte_to_lsb(file_extn[i], buffer.data());

        // write 8 bytes buffer to stego file
        encInfo.fptr_stego_image.write(buffer.data(), 8);
        if (!encInfo.fptr_stego_image)
            return e_failure;
    }
    return e_success;
}

Status encode_secret_file_size(long file_size, EncodeInfo &encInfo)
{
    vector<char> buffer(32);

    // read 32 bytes from src image to store buffer
    encInfo.fptr_src_image.read(buffer.data(), 32);
    if (encInfo.fptr_src_image.gcount() != 32)
        return e_failure;

    encode_size_to_lsb(file_size, buffer.data());

    // write 32 bytes buffer to stego file
    encInfo.fptr_stego_image.write(buffer.data(), 32);
    if (!encInfo.fptr_stego_image)
        return e_failure;

    return e_success;
}

// Encode function, which does the real encoding - generic function
Status encode_data_to_image(char *data, int size, fstream &fptr_src_image, fstream &fptr_stego_image)
{
    // For each byte in data
    for (int i = 0; i < size; i++)
    {
        vector<char> buffer(8);

        // Read 8 bytes from src image
        fptr_src_image.read(buffer.data(), 8);
        if (fptr_src_image.gcount() != 8)
            return e_failure;

        // Hide that byte in LSB
        encode_byte_to_lsb(data[i], buffer.data());

        // Write 8 bytes to stego image
        fptr_stego_image.write(buffer.data(), 8);
        if (!fptr_stego_image)
            return e_failure;
    }
    return e_success;
}

Status encode_secret_file_data(EncodeInfo &encInfo)
{
    // bytes encoded
    int bytes_encoded = 0;
    vector<char> buffer(8);
    char ch;

    // seek set 0 to sec data
    encInfo.fptr_secret.seekg(0, ios::beg);

    // read 1 byte from sec
    while (encInfo.fptr_secret.get(ch))
    {
        // read 8 byte from src image
        encInfo.fptr_src_image.read(buffer.data(), 8);
        if (encInfo.fptr_src_image.gcount() != 8)
            return e_failure;

        encode_byte_to_lsb(ch, buffer.data());

        // write buffer to stego file
        encInfo.fptr_stego_image.write(buffer.data(), 8);
        if (!encInfo.fptr_stego_image)
            return e_failure;

        bytes_encoded++;
    }
    cout << "INFO: Total bytes encoded from secret file: " << bytes_encoded << endl;
    return e_success;
}

Status copy_remaining_img_data(fstream &fptr_src, fstream &fptr_dest)
{
    char ch;

    // read 1 byte from src file
    while (fptr_src.get(ch))
    {
        // write 1 byte to dest file
        fptr_dest.put(ch);
        if (!fptr_dest)
            return e_failure;
    }
    return e_success;
}