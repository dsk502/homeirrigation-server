
#include "crypto/rsa_utils.hpp"

//Print openssl error info
void print_openssl_error() {
    ERR_print_errors_fp(stderr);
}

//Base64 decoding
std::vector<unsigned char> RSAUtils::base64_decode(const std::string& base64_data) {
    BIO* bio = BIO_new_mem_buf(base64_data.c_str(), -1);
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Don't use carry
    std::vector<unsigned char> der_data;
    der_data.resize(BIO_get_mem_data(bio, nullptr));

    int len = BIO_read(bio, der_data.data(), base64_data.size());
    if (len <= 0) {
        print_openssl_error();
        BIO_free_all(bio);
        return {};
    }

    der_data.resize(len);
    BIO_free_all(bio);
    return der_data;
}

//Base64 encoding
std::string RSAUtils::base64_encode(const unsigned char* data, size_t length) {
    BIO* bio = BIO_new(BIO_s_mem());
    BIO* b64 = BIO_new(BIO_f_base64());
    bio = BIO_push(b64, bio);

    BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Don't use carry
    BIO_write(bio, data, length);
    BIO_flush(bio);

    BUF_MEM* bptr;
    BIO_get_mem_ptr(bio, &bptr);
    std::string base64_data(bptr->data, bptr->length);

    BIO_free_all(bio);
    return base64_data;
}

// Determine whether keypair files exist on the disk
bool RSAUtils::is_keypair_exist() {
    std::ifstream prikey_file(SERVER_PRIKEY_FILE);
    std::ifstream pubkey_file(SERVER_PUBKEY_FILE);
    if (prikey_file.is_open() && pubkey_file.is_open()) {
        prikey_file.close();
        pubkey_file.close();
        return true;
    } else {
        prikey_file.close();
        pubkey_file.close();
        return false;
    }
}

// Generate the key pair and store the base64-encoded der format to the files
int RSAUtils::generate_der_base64_key_pair(int bits) {
    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (!ctx) {
        print_openssl_error();
        throw std::runtime_error("Failed to create EVP_PKEY_CTX.");
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        throw std::runtime_error("Failed to initialize key generation.");
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, bits) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        throw std::runtime_error("Failed to set RSA keygen bits.");
    }

    EVP_PKEY* pkey = nullptr;
    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        throw std::runtime_error("Failed to generate RSA key pair.");
    }
    EVP_PKEY_CTX_free(ctx);

    //Get private key (DER format)
    std::vector<unsigned char> der_private_key;
    unsigned char* der_private = nullptr;
    int der_private_len = i2d_PrivateKey(pkey, &der_private);
    if (der_private_len <= 0) {
        EVP_PKEY_free(pkey);
        print_openssl_error();
        throw std::runtime_error("Failed to convert private key to DER format.");
    }
    der_private_key.assign(der_private, der_private + der_private_len);
    OPENSSL_free(der_private);

    //Get public key (DER format)
    std::vector<unsigned char> der_public_key;
    unsigned char* der_public = nullptr;
    int der_public_len = i2d_PUBKEY(pkey, &der_public);
    if (der_public_len <= 0) {
        EVP_PKEY_free(pkey);
        print_openssl_error();
        throw std::runtime_error("Failed to convert public key to DER format.");
    }
    der_public_key.assign(der_public, der_public + der_public_len);
    OPENSSL_free(der_public);

    EVP_PKEY_free(pkey);

    //Base64 encoding
    std::string base64_private_key = base64_encode(der_private_key.data(), der_private_key.size());
    std::string base64_public_key = base64_encode(der_public_key.data(), der_public_key.size());

    //Write the key pair to the files
    std::ofstream private_key_file(SERVER_PRIKEY_FILE);
    private_key_file << base64_private_key;
    private_key_file.close();

    std::ofstream public_key_file(SERVER_PUBKEY_FILE);
    public_key_file << base64_public_key;
    public_key_file.close();

    return 0;
}

//Load base64-encoded server public key from file
EVP_PKEY* RSAUtils::load_base64_der_server_pubkey() {
    std::ifstream public_key_file(SERVER_PUBKEY_FILE);
    std::string base64_pubkey;
    if (std::getline(public_key_file, base64_pubkey)) {
        public_key_file.close();
    } else {
        public_key_file.close();
        std::cerr << "Failed to read the server public key file" << std::endl;
        return nullptr;
    }

    std::vector<unsigned char> der_pubkey = base64_decode(base64_pubkey);
    if (der_pubkey.empty()) {
        std::cerr << "Failed to decode Base64 DER public key." << std::endl;
        return nullptr;
    }

    const unsigned char* der_ptr = der_pubkey.data();
    EVP_PKEY* pkey = d2i_PUBKEY(nullptr, &der_ptr, der_pubkey.size());
    if (!pkey) {
        print_openssl_error();
        return nullptr;
    }

    return pkey;
}

//Load base64-encoded server private key from file
EVP_PKEY* RSAUtils::load_base64_der_server_prikey() {
    std::ifstream private_key_file(SERVER_PRIKEY_FILE);
    std::string base64_privatekey;
    if (std::getline(private_key_file, base64_privatekey)) {
        private_key_file.close();
    } else {
        private_key_file.close();
        std::cerr << "Failed to read the server private key file" << std::endl;
        return nullptr;
    }

    std::vector<unsigned char> der_privatekey = base64_decode(base64_privatekey);
    if (der_privatekey.empty()) {
        std::cerr << "Failed to decode Base64 DER private key." << std::endl;
        return nullptr;
    }

    const unsigned char* der_ptr = der_privatekey.data();
    EVP_PKEY* pkey = nullptr;
    d2i_PrivateKey(EVP_PKEY_RSA, &pkey, &der_ptr, der_privatekey.size());
    if (!pkey) {
        print_openssl_error();
        return nullptr;
    }

    return pkey;
}

// Load client public key from string
EVP_PKEY* RSAUtils::load_base64_der_client_pubkey(std::string key_str) {
    std::vector<unsigned char> der_pubkey = base64_decode(key_str);
    if (der_pubkey.empty()) {
        std::cerr << "Failed to decode Base64 DER public key." << std::endl;
        return nullptr;
    }

    const unsigned char* der_ptr = der_pubkey.data();
    EVP_PKEY* pkey = d2i_PUBKEY(nullptr, &der_ptr, der_pubkey.size());
    if (!pkey) {
        print_openssl_error();
        return nullptr;
    }

    return pkey;
}

//RSA Encryption
std::string RSAUtils::rsa_encrypt(EVP_PKEY* pkey, const std::string& data) {
    if (!pkey) {
        print_openssl_error();
        return "";
    }

    int rsa_size = EVP_PKEY_size(pkey);
    std::vector<unsigned char> encrypted_data(rsa_size);

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        print_openssl_error();
        return "";
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        return "";
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        return "";
    }

    size_t outlen = 0;
    if (EVP_PKEY_encrypt(ctx, nullptr, &outlen, reinterpret_cast<const unsigned char*>(data.data()), data.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        return "";
    }

    if (EVP_PKEY_encrypt(ctx, encrypted_data.data(), &outlen, reinterpret_cast<const unsigned char*>(data.data()), data.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        return "";
    }

    encrypted_data.resize(outlen);
    EVP_PKEY_CTX_free(ctx);

    return base64_encode(encrypted_data.data(), encrypted_data.size());
}

//RSA decryption
std::string RSAUtils::rsa_decrypt(EVP_PKEY* pkey, const std::string& encrypted_data) {
    if (!pkey) {
        print_openssl_error();
        return "";
    }

    //std::cout << encrypted_data <<std::endl;
    std::vector<unsigned char> der_encrypted_data = base64_decode(encrypted_data);
    if (der_encrypted_data.empty()) {
        std::cerr << "Failed to decode Base64 encrypted data." << std::endl;
        return "";
    }

    int rsa_size = EVP_PKEY_size(pkey);
    std::vector<unsigned char> decrypted_data(rsa_size);

    EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(pkey, nullptr);
    if (!ctx) {
        print_openssl_error();
        return "";
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        return "";
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_PADDING) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        return "";
    }

    size_t outlen = 0;
    if (EVP_PKEY_decrypt(ctx, nullptr, &outlen, der_encrypted_data.data(), der_encrypted_data.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        return "";
    }

    if (EVP_PKEY_decrypt(ctx, decrypted_data.data(), &outlen, der_encrypted_data.data(), der_encrypted_data.size()) <= 0) {
        EVP_PKEY_CTX_free(ctx);
        print_openssl_error();
        return "";
    }

    decrypted_data.resize(outlen);
    EVP_PKEY_CTX_free(ctx);

    return std::string(decrypted_data.begin(), decrypted_data.end());
}

std::string RSAUtils::read_key_from_file(bool is_pubkey) {
    std::ifstream key_file;
    if(is_pubkey) {
        //Read public key file "server_pubkey.der"
        key_file = std::ifstream("keys/server_pubkey.der");
    } else {
        //Read private key file "server_prikey.der"
        key_file = std::ifstream("keys/server_prikey.der");
        
    }
    if (!key_file.is_open()) {
        //If the file does not exist, then is_open() will return false.
        std::cerr << "Failed to open the file" << std::endl;
        return "";
    }
    std::string key;
    if (std::getline(key_file, key)) {
        key_file.close();
        return key;
    } else {
        key_file.close();
        std::cerr << "Failed to read the key file" << std::endl;
        return "";
    }
}