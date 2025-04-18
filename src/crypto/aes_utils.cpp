#include "crypto/aes_utils.hpp"

#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/buffer.h>

#define KEY_LEN 32
#define IV_LEN 16
#define KEY_FILE "keys/aes.key"

//Generate AES key and IV, and save to file
void AESUtils::generate_key_iv() {
    //const unsigned int key_len = 32; // AES-256 key length
    //const unsigned int iv_len = 16;  // IV length
    unsigned char key[KEY_LEN], iv[IV_LEN];

    //Generate AES key and IV
    RAND_bytes(key, KEY_LEN);
    RAND_bytes(iv, IV_LEN);

    //Save key and IV to the file
    std::ofstream key_file_stream(KEY_FILE, std::ios::binary);
    if (!key_file_stream) {
        std::cerr << "Failed to create the AES key file" << KEY_FILE << std::endl;
        return;
    }
    key_file_stream.write(reinterpret_cast<char*>(key), KEY_LEN);
    key_file_stream.write(reinterpret_cast<char*>(iv), IV_LEN);
    key_file_stream.close();
    std::cout << "AES key and IV are saved to file: " << KEY_FILE << std::endl;
}

//Use ifstream to determine the existance of the file
bool AESUtils::is_key_file_exist() {
    std::ifstream key_file(KEY_FILE, std::ios::binary);
    return key_file.good();
}

//Read base64-encoded key from file to string
std::string AESUtils::read_key_base64() {
    return read_base64(KEY_LEN, 0);
}

//Read base64-encoded IV from file to string
std::string AESUtils::read_iv_base64() {
    return read_base64(IV_LEN, KEY_LEN);
}

//Read base64
std::string AESUtils::read_base64(size_t length, size_t offset) {
    std::ifstream key_file(KEY_FILE, std::ios::binary);
    if (!key_file) {
        std::cerr << "Failed to open the AES key file:" << KEY_FILE << std::endl;
        return "";
    }

    //Go after the offset
    key_file.seekg(offset);
    if (!key_file) {
        std::cerr << "Failed to locate the position after the offset" << std::endl;
        return "";
    }

    //Read data
    std::vector<unsigned char> data(length);
    key_file.read(reinterpret_cast<char*>(data.data()), length);
    if (!key_file) {
        std::cerr << "Failed to read the data" << std::endl;
        return "";
    }

    //Base64 encoding
    return base64_encode(data.data(), length);
}

//Base64 encoding
std::string AESUtils::base64_encode(const unsigned char* data, size_t length) {
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); // Don't use carry
    BIO_write(bio, data, length);
    BIO_flush(bio);

    BUF_MEM* buffer_ptr;
    BIO_get_mem_ptr(bio, &buffer_ptr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    return std::string(buffer_ptr->data, buffer_ptr->length);
}

//Encrypt the file
void AESUtils::encrypt_file(std::string server_id) {
    //const unsigned int key_len = 32; // AES-256 key length
    //const unsigned int iv_len = 16;  // IV length
    unsigned char key[KEY_LEN], iv[IV_LEN];

    //Read key and IV from the file
    std::ifstream key_file_stream(KEY_FILE, std::ios::binary);
    if (!key_file_stream) {
        std::cerr << "Cannot open the AES key file " << KEY_FILE << std::endl;
        return;
    }
    key_file_stream.read(reinterpret_cast<char*>(key), KEY_LEN);
    key_file_stream.read(reinterpret_cast<char*>(iv), IV_LEN);
    key_file_stream.close();

    std::string input_file = "dbs/watering_record.db";
    
    //Open the input file
    std::ifstream infile(input_file, std::ios::binary);
    if (!infile) {
        std::cerr << "Cannot open file: " << input_file << std::endl;
        return;
    }

    //Open the output file
    std::string output_file = "temp/watering_record_" + server_id + "_encrypted.db";
    std::ofstream outfile(output_file, std::ios::binary);
    if (!outfile) {
        std::cerr << "Cannot create the output file" << std::endl;
        return;
    }

    //Init the encryption context
    EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv);

    //Encrypt the file
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

    //End the encryption
    EVP_EncryptFinal_ex(ctx, outbuf, &outlen);
    outfile.write(reinterpret_cast<char*>(outbuf), outlen);

    //Clean the resources
    EVP_CIPHER_CTX_free(ctx);
    infile.close();
    outfile.close();
    std::cout << "File encryption succeed: " << output_file << std::endl;
}