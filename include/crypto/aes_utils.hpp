#ifndef AESUTILS_HPP
#define AESUTILS_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <openssl/evp.h>
#include <openssl/rand.h>

class AESUtils {
public:
    static void generate_key_iv();
    static bool is_key_file_exist();
    static std::string read_key_base64();
    static std::string read_iv_base64();
    static std::string read_base64(size_t length, size_t offset);
    static std::string base64_encode(const unsigned char* data, size_t length);
    static void encrypt_file(std::string server_id);
};

#endif