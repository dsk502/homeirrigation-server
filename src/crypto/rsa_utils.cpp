#include <iostream>
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

// 加载Base64编码的公钥
RSA* load_base64_public_key(const std::string& base64_pubkey)
{
    std::vector<unsigned char> der_pubkey = base64_decode(base64_pubkey);
    if (der_pubkey.empty())
    {
        std::cerr << "Failed to decode Base64 public key." << std::endl;
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

// 加载Base64编码的私钥
RSA* load_base64_private_key(const std::string& base64_privatekey)
{
    std::vector<unsigned char> der_privatekey = base64_decode(base64_privatekey);
    if (der_privatekey.empty())
    {
        std::cerr << "Failed to decode Base64 private key." << std::endl;
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

int main()
{
    // 初始化OpenSSL
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
    OPENSSL_config(nullptr);

    // 示例Base64编码的公钥和私钥（替换为实际值）
}