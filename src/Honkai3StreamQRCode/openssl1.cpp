#pragma warning(disable : 4996)
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <string>
#include <iostream>
using namespace std;
#include"CryptoKit.h"
string rsacreate(string message, string public_key) {
    // Load public key
    RSA* rsa = nullptr;
    BIO* bio = BIO_new_mem_buf(public_key.c_str(), public_key.length());
    rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, nullptr, nullptr);
    BIO_free(bio);
    if (!rsa) {
        cerr << "Failed to load public key" << endl;
        return "";
    }

    // Encrypt message
    const int RSA_PADDING = RSA_PKCS1_PADDING;
    unsigned char* encrypted = new unsigned char[RSA_size(rsa)];
    int len = RSA_public_encrypt(message.length(), (unsigned char*)message.c_str(), encrypted, rsa, RSA_PADDING);
    if (len < 0) {
        cerr << "Failed to encrypt message" << endl;
        RSA_free(rsa);
        delete[] encrypted;
        return "";
    }

    // Convert encrypted message to base64
    BIO* bin = BIO_new_mem_buf((void*)encrypted, -1);
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* bmem = BIO_new(BIO_s_mem());
    b64 = BIO_push(b64, bmem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, encrypted, len);
    BIO_flush(b64);
    BUF_MEM* bptr;
    BIO_get_mem_ptr(b64, &bptr);
    string cipher_text(bptr->data, bptr->length);

    // Clean up
    RSA_free(rsa);
    delete[] encrypted;
    BIO_free_all(b64);

    return cipher_text;
}


int main()
{
    CryptoKit c;
    string s = c.rsaEncrypt("466b04379f3c549bbl20221123", "-----BEGIN PUBLIC KEY-----\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDjb4V7EidX/ym28t2ybo0U6t0n\n6p4ej8VjqKHg100va6jkNbNTrLQqMCQCAYtXMXXp2Fwkk6WR+12N9zknLjf+C9sx\n/+l48mjUU8RqahiFD1XT/u2e0m2EN029OhCgkHx3Fc/KlFSIbak93EH/XlYis0w+\nXl69GV6klzgxW6d2xQIDAQAB\n-----END PUBLIC KEY-----");
    cout << s << endl;
    cout << rsacreate("466b04379f3c549bbl20221123", "-----BEGIN PUBLIC KEY-----\nMIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQDjb4V7EidX/ym28t2ybo0U6t0n\n6p4ej8VjqKHg100va6jkNbNTrLQqMCQCAYtXMXXp2Fwkk6WR+12N9zknLjf+C9sx\n/+l48mjUU8RqahiFD1XT/u2e0m2EN029OhCgkHx3Fc/KlFSIbak93EH/XlYis0w+\nXl69GV6klzgxW6d2xQIDAQAB\n-----END PUBLIC KEY-----") << endl;
}