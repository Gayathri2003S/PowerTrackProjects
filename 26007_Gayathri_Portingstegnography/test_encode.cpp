#include <iostream>
#include "encode.h"
#include "type.h"
#include "decode.h"

using namespace std;

int main(int argc, char *argv[])
{ 
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    // Check operation type (encode / decode)
    int ret = check_operation_type(argv);

    // Check the condition
    if (ret == e_encode)
    {
        // Encoding
        int ret = read_and_validate_encode_args(argv, encInfo);
        if (ret == e_failure)
        {
            cout << "Validation is Invalid." << endl;
            return 0;
        }
        ret = do_encoding(encInfo);
        if (ret == e_failure)
        {
            cout << "Encoding is failed" << endl;
            return 0;
        }
        cout << "Encoding is successful...." << endl;
        return 0;
    }
    else if (ret == e_decode)
    {
        // Decoding
        if (read_validate_decode_args(argv, decInfo) == e_failure)
        {
            cout << "Validation is failed" << endl;
            return 0;
        }

        if (do_decoding(decInfo) == e_failure)
        {
            cout << "Decoding is failed" << endl;
            return 0;
        }
        cout << "Decoding is successful!!" << endl;
        return 0;
    }
    else
    {
        // Unsupported
        cout << "Invalid operation type" << endl;
        return 0;
    }
}