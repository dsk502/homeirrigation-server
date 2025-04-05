#ifndef RSAUTILS_HPP
#define RSAUTILS_HPP

#include <iostream>
#include <fstream>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <vector>
#include <string>

#define SERVER_PUBKEY_FILE "keys/server_pubkey.der"
#define SERVER_PRIKEY_FILE "keys/server_prikey.der"

class RSAUtils {
public:
    static bool is_keypair_exist();
    static int generate_der_base64_key_pair(int bits = 2048);
    static RSA* load_base64_der_client_pubkey(std::string key_str);
    static RSA* load_base64_der_server_pubkey();
    static RSA* load_base64_der_server_prikey();
    static std::string rsa_encrypt(RSA* rsa, const std::string& data);
    static std::string rsa_decrypt(RSA* rsa, const std::string& encrypted_data)
    static std::string read_key_from_file(bool is_pubkey);

private:
    static std::vector<unsigned char> base64_decode(const std::string& base64_data);
    static std::string base64_encode(const unsigned char* data, size_t length);
};

#endif