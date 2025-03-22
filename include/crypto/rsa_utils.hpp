#include <iostream>
#include <fstream>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <vector>
#include <string>

class RSAUtils {
public:
    static std::vector<unsigned char> base64_decode(const std::string& base64_data);
    static std::string base64_encode(const unsigned char* data, size_t length);
    static std::pair<std::string, std::string> generate_der_base64_key_pair(int bits = 2048);
    static RSA* load_base64_der_public_key(const std::string& base64_pubkey);
    static RSA* load_base64_der_private_key(const std::string& base64_privatekey);
    static std::vector<unsigned char> rsa_encrypt(RSA* rsa, const std::vector<unsigned char>& data);
    static std::vector<unsigned char> rsa_decrypt(RSA* rsa, const std::vector<unsigned char>& encrypted_data);
    static std::string read_key_from_file(bool is_pubkey);
    static int write_key_to_file(bool is_pubkey, std:string key);
};