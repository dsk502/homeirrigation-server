#include <iostream>
#include <fstream>
#include <openssl/evp.h>
#include <openssl/rand.h>

void encrypt_file(const std::string& file) {
    const unsigned int key_len = 32; // AES-256 key length
    const unsigned int iv_len = 16;  // IV length
    unsigned char key[key_len], iv[iv_len];

    // 生成密钥和IV
    RAND_bytes(key, key_len);
    RAND_bytes(iv, iv_len);

    // 保存密钥和IV到文件
    std::ofstream key_file(file + ".key", std::ios::binary);
    key_file.write(reinterpret_cast<char*>(key), key_len);
    key_file.write(reinterpret_cast<char*>(iv), iv_len);
    key_file.close();

    // 打开输入文件
    std::ifstream infile(file, std::ios::binary);
    if (!infile) {
        std::cerr << "无法打开文件: " << file << std::endl;
        return;
    }

    // 打开输出文件
    std::ofstream outfile(file + ".enc", std::ios::binary);
    if (!outfile) {
        std::cerr << "无法新建输出文件." << std::endl;
        return;
    }

    // 初始化加密上下文
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    // 加密文件
    unsigned char inbuf[1024], outbuf[1024 + EVP_MAX_BLOCK_LENGTH];
    int inlen, outlen;
    while (infile.good()) {
        infile.read(reinterpret_cast<char*>(inbuf), sizeof(inbuf));
        inlen = infile.gcount();
        if (inlen > 0) {
            EVP_EncryptUpdate(ctx, outbuf, &outlen, inbuf, inlen);
            outfile.write(reinterpret_cast<char*>(outbuf), outlen);
        }
    }

    // 结束加密
    EVP_EncryptFinal_ex(ctx, outbuf, &outlen);
    outfile.write(reinterpret_cast<char*>(outbuf), outlen);

    // 清理
    EVP_CIPHER_CTX_free(ctx);
    infile.close();
    outfile.close();
    std::cout << "文件加密成功: " << file + ".enc" << std::endl;
}

int main() {
    std::string file = "example.txt";
    encrypt_file(file);
    return 0;
}