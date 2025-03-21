#include <iostream>
#include <fstream>
#include <openssl/rsa.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <vector>
#include <string>

// 打印OpenSSL错误信息
void print_openssl_error()
{
    ERR_print_errors_fp(stderr);
}

// Base64解码函数
std::vector<unsigned char> base64_decode(const std::string& base64_data)
{
    BIO* bio = BIO_new_mem_buf(base64_data.c_str(), -1);
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // 忽略换行符
    std::vector<unsigned char> der_data;
    der_data.resize(BIO_get_mem_data(bio, nullptr));

    int len = BIO_read(bio, der_data.data(), base64_data.size());
    if (len <= 0)
    {
        print_openssl_error();
        BIO_free_all(bio);
        return {};
    }

    der_data.resize(len);
    BIO_free_all(bio);
    return der_data;
}

// Base64编码函数
std::string base64_encode(const unsigned char* data, size_t length)
{
    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // 忽略换行符
    BIO_write(bio, data, length);
    BIO_flush(bio);

    BUF_MEM* bptr;
    BIO_get_mem_ptr(bio, &bptr);
    std::string base64_data(bptr->data, bptr->length);

    BIO_free_all(bio);
    return base64_data;
}

// 生成DER格式并Base64编码的密钥对
std::pair<std::string, std::string> generate_der_base64_key_pair(int bits = 2048)
{
    RSA* rsa = RSA_generate_key(bits, RSA_F4, nullptr, nullptr);
    if (!rsa)
    {
        print_openssl_error();
        throw std::runtime_error("Failed to generate RSA key pair.");
    }

    // 获取私钥（DER格式）
    std::vector<unsigned char> der_private_key(i2d_RSAPrivateKey(rsa, nullptr));
    unsigned char* p = der_private_key.data();
    i2d_RSAPrivateKey(rsa, &p);

    // 获取公钥（DER格式）
    std::vector<unsigned char> der_public_key(i2d_RSA_PUBKEY(rsa, nullptr));
    p = der_public_key.data();
    i2d_RSA_PUBKEY(rsa, &p);

    // Base64编码
    std::string base64_private_key = base64_encode(der_private_key.data(), der_private_key.size());
    std::string base64_public_key = base64_encode(der_public_key.data(), der_public_key.size());

    RSA_free(rsa);

    return {base64_private_key, base64_public_key};
}

// 加载Base64编码的DER格式公钥
RSA* load_base64_der_public_key(const std::string& base64_pubkey)
{
    std::vector<unsigned char> der_pubkey = base64_decode(base64_pubkey);
    if (der_pubkey.empty())
    {
        std::cerr << "Failed to decode Base64 DER public key." << std::endl;
        return nullptr;
    }

    const unsigned char* der_ptr = der_pubkey.data();
    RSA* rsa = d2i_RSA_PUBKEY(nullptr, &der_ptr, der_pubkey.size());
    if (!rsa)
    {
        print_openssl_error();
        return nullptr;
    }
    return rsa;
}

// 加载Base64编码的DER格式私钥
RSA* load_base64_der_private_key(const std::string& base64_privatekey)
{
    std::vector<unsigned char> der_privatekey = base64_decode(base64_privatekey);
    if (der_privatekey.empty())
    {
        std::cerr << "Failed to decode Base64 DER private key." << std::endl;
        return nullptr;
    }

    const unsigned char* der_ptr = der_privatekey.data();
    RSA* rsa = d2i_RSAPrivateKey(nullptr, &der_ptr, der_privatekey.size());
    if (!rsa)
    {
        print_openssl_error();
        return nullptr;
    }
    return rsa;
}

// RSA加密
std::vector<unsigned char> rsa_encrypt(RSA* rsa, const std::vector<unsigned char>& data)
{
    int rsa_size = RSA_size(rsa);
    std::vector<unsigned char> encrypted_data(rsa_size);

    int result = RSA_public_encrypt(data.size(), data.data(), encrypted_data.data(), rsa, RSA_PKCS1_PADDING);
    if (result == -1)
    {
        print_openssl_error();
        return {};
    }

    encrypted_data.resize(result);
    return encrypted_data;
}

// RSA解密
std::vector<unsigned char> rsa_decrypt(RSA* rsa, const std::vector<unsigned char>& encrypted_data)
{
    int rsa_size = RSA_size(rsa);
    std::vector<unsigned char> decrypted_data(rsa_size);

    int result = RSA_private_decrypt(encrypted_data.size(), encrypted_data.data(), decrypted_data.data(), rsa, RSA_PKCS1_PADDING);
    if (result == -1)
    {
        print_openssl_error();
        return {};
    }

    decrypted_data.resize(result);
    return decrypted_data;
}

/*
int main()
{
    // 生成密钥对
    try
    {
        auto [base64_private_key, base64_public_key] = generate_der_base64_key_pair();
        std::cout << "Base64 Encoded Private Key (DER format):\n" << base64_private_key << std::endl;
        std::cout << "Base64 Encoded Public Key (DER format):\n" << base64_public_key << std::endl;

        // 加载Base64编码的密钥
        RSA* rsa_private = load_base64_der_private_key(base64_private_key);
        RSA* rsa_public = load_base64_der_public_key(base64_public_key);

        if (!rsa_private || !rsa_public)
        {
            std::cerr << "Failed to load keys." << std::endl;
            return 1;
        }

        // 测试加密和解密
        std::vector<unsigned char> data = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd'};
        std::vector<unsigned char> encrypted_data = rsa_encrypt(rsa_public, data);
        std::vector<unsigned char> decrypted_data = rsa_decrypt(rsa_private, encrypted_data);

        std::cout << "Original Data: ";
        for (auto ch : data)
        {
            std::cout << ch;
        }
        std::cout << std::endl;

        std::cout << "Encrypted Data (hex): ";
        for (auto ch : encrypted_data)
        {
            std::cout << std::hex << (int)ch << " ";
        }
        std::cout << std::endl;

        std::cout << "Decrypted Data: ";
        for (auto ch : decrypted_data)
        {
            std::cout << ch;
        }
        std::cout << std::endl;

        RSA_free(rsa_private);
        RSA_free(rsa_public);
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
*/

std::string read_key_from_file(bool is_pubkey) {
    std::ifstream key_file;
    if(is_pubkey) {
        //Read public key file "server_pubkey.der"
        key_file = std::ifstream("keys/server_pubkey.der");
    } else {
        //Read private key file "server_prikey.der"
        key_file = std::ifstream("keys/server_prikey.der"); // 打开文件
        
    }
    if (!key_file.is_open()) {
        std::cerr << "Failed to open the file" << std::endl;
        return "";
    }
    std::string key;
    if (std::getline(key_file, key)) { // 读取第一行
        key_file.close();
        return key;
    } else {
        key_file.close();
        std::cerr << "Failed to read the file" << std::endl;
        return "";
    }
}

int write_key_to_file(bool is_pubkey, std:string key) {
    std::ofstream key_file;
    if(is_pubkey) {
        key_file = std::ofstream("keys/server_pubkey.der")
    } else {
        key_file = std::ofstream("keys/server_prikey.der")
    }

    if (!key_file.is_open()) {
        std::cerr << "Failed to open the file" << std::endl;
        return -1;
    }

    // 写入新的第一行内容
    key_file << key;

    // 关闭文件
    key_file.close();

    return 0;
}