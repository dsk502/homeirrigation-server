#include "crypto/aes_utils.hpp"

#define KEY_LEN 32
#define IV_LEN 16
#define KEY_FILE "keys/aes.key"

// 生成密钥和IV，并保存到文件
void AESUtils::generate_key_iv() {
    //const unsigned int key_len = 32; // AES-256 key length
    //const unsigned int iv_len = 16;  // IV length
    unsigned char key[KEY_LEN], iv[IV_LEN];

    // 生成密钥和IV
    RAND_bytes(key, KEY_LEN);
    RAND_bytes(iv, IV_LEN);

    // 保存密钥和IV到文件
    std::ofstream key_file_stream(KEY_FILE, std::ios::binary);
    if (!key_file_stream) {
        std::cerr << "无法创建密钥文件: " << KEY_FILE << std::endl;
        return;
    }
    key_file_stream.write(reinterpret_cast<char*>(key), KEY_LEN);
    key_file_stream.write(reinterpret_cast<char*>(iv), IV_LEN);
    key_file_stream.close();
    std::cout << "密钥和IV已保存到文件: " << KEY_FILE << std::endl;
}

//Use ifstream to determine the existance of the file
bool AESUtils::is_key_file_exist() {
    std::ifstream key_file(KEY_FILE, std::ios::binary);
    return key_file.good();
}

// 读取密钥的Base64编码形式
std::string AESUtils::read_key_base64() {
    return read_base64(KEY_LEN, 0);
}

// 读取IV的Base64编码形式
std::string AESUtils::read_iv_base64() {
    return read_base64(IV_LEN, KEY_LEN);
}

// 通用函数：读取指定部分的Base64编码
std::string AESUtils::read_base64(size_t length, size_t offset) {
    std::ifstream key_file(KEY_FILE, std::ios::binary);
    if (!key_file) {
        std::cerr << "无法打开密钥文件: " << KEY_FILE << std::endl;
        return "";
    }

    // 跳过偏移量
    key_file.seekg(offset);
    if (!key_file) {
        std::cerr << "无法定位到文件中的指定位置." << std::endl;
        return "";
    }

    // 读取指定长度的数据
    std::vector<unsigned char> data(length);
    key_file.read(reinterpret_cast<char*>(data.data()), length);
    if (!key_file) {
        std::cerr << "无法读取文件中的数据." << std::endl;
        return "";
    }

    // Base64编码
    return base64_encode(data.data(), length);
}

// Base64编码
std::string AESUtils::base64_encode(const unsigned char* data, size_t length) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // 禁用换行符
    BIO_write(bio, data, length);
    BIO_flush(bio);

    BUF_MEM* buffer_ptr;
    BIO_get_mem_ptr(bio, &buffer_ptr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    return std::string(buffer_ptr->data, buffer_ptr->length);
}

// 加密文件
void AESUtils::encrypt_file(std::string server_id) {
    //const unsigned int key_len = 32; // AES-256 key length
    //const unsigned int iv_len = 16;  // IV length
    unsigned char key[KEY_LEN], iv[IV_LEN];

    // 从密钥文件中读取密钥和IV
    std::ifstream key_file_stream(KEY_FILE, std::ios::binary);
    if (!key_file_stream) {
        std::cerr << "无法打开密钥文件: " << KEY_FILE << std::endl;
        return;
    }
    key_file_stream.read(reinterpret_cast<char*>(key), KEY_LEN);
    key_file_stream.read(reinterpret_cast<char*>(iv), IV_LEN);
    key_file_stream.close();

    std::string input_file = "dbs/watering_record.db";
    
    // 打开输入文件
    std::ifstream infile(input_file, std::ios::binary);
    if (!infile) {
        std::cerr << "无法打开文件: " << input_file << std::endl;
        return;
    }

    // 打开输出文件
    std::string output_file = "temp/watering_record_" + server_id + "_encrypted.db";
    std::ofstream outfile(output_file, std::ios::binary);
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
    std::cout << "文件加密成功: " << output_file << std::endl;
}