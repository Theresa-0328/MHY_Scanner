#include "CryptoKit.h"

#include <sstream>
#include <iomanip>
#include <iostream>

#ifdef USE_OPENSSL
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/buffer.h>
#include <openssl/hmac.h>
#include <openssl/md5.h>
#endif // OPENSSL

#ifdef USE_OPENSSL

#pragma warning(disable : 4996)

std::string rsaEncrypt(const std::string& message, const std::string& public_key)
{
	RSA* rsa = nullptr;
	BIO* bio = BIO_new_mem_buf(public_key.c_str(), (int)public_key.length());
	rsa = PEM_read_bio_RSA_PUBKEY(bio, &rsa, nullptr, nullptr);
	BIO_free(bio);
	if (!rsa)
	{
		std::cerr << "Failed to load public key" << std::endl;
		return "";
	}

	// Encrypt message
	const int RSA_PADDING = RSA_PKCS1_PADDING;
	unsigned char* encrypted = new unsigned char[RSA_size(rsa)];
	int len = RSA_public_encrypt((int)message.length(), (unsigned char*)message.c_str(), encrypted, rsa, RSA_PADDING);
	if (len < 0)
	{
		std::cerr << "Failed to encrypt message" << std::endl;
		RSA_free(rsa);
		delete[] encrypted;
		return "";
	}

	// Convert encrypted message to base64
	//BIO* bin = BIO_new_mem_buf((void*)encrypted, -1);
	BIO* b64 = BIO_new(BIO_f_base64());
	BIO* bmem = BIO_new(BIO_s_mem());
	b64 = BIO_push(b64, bmem);
	BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(b64, encrypted, len);
	BIO_flush(b64);
	BUF_MEM* bptr;
	BIO_get_mem_ptr(b64, &bptr);
	std::string cipher_text(bptr->data, bptr->length);

	// Clean up
	RSA_free(rsa);
	delete[] encrypted;
	BIO_free_all(b64);

	return cipher_text;
}

void FormatRsaPublicKey(std::string& key)
{
	// 检查输入参数合法性
	if (key.empty())
	{
		throw std::invalid_argument("input key is empty");
	}
	size_t beginPos = key.find("-----BEGIN PUBLIC KEY-----");
	if (beginPos == std::string::npos)
	{
		throw std::invalid_argument("input key does not contain BEGIN PUBLIC KEY marker");
	}
	size_t endPos = key.find("-----END PUBLIC KEY-----");
	if (endPos == std::string::npos)
	{
		throw std::invalid_argument("input key does not contain END PUBLIC KEY marker");
	}

	std::string publicKey = key.substr(beginPos + 26, endPos - 26);
	std::stringstream ss;

	ss << "-----BEGIN PUBLIC KEY-----\n";
	for (size_t i = 0; i < publicKey.length(); i += 64)
	{
		ss << publicKey.substr(i, 64) << "\n";
	}
	ss << "-----END PUBLIC KEY-----";
	key = ss.str();
	return;
}

std::string HmacSha256(const std::string& message, const std::string& key)
{
	const EVP_MD* evp_md = EVP_sha256();
	unsigned int md_len = EVP_MD_size(evp_md);
	unsigned char* md = new unsigned char[md_len];
	unsigned int hmac_len;
	unsigned char* hmac = HMAC(evp_md, key.c_str(), (int)key.length(), (const unsigned char*)message.c_str(), message.length(), md, &hmac_len);
	std::string result((char*)hmac, hmac_len);
	std::string output = "";
	char hex[3] = { 0 };
	for (unsigned char c : result)//HEX
	{
		sprintf_s(hex, "%02x", c);
		output += hex;
	}
	delete[] md;
	return output;
}

std::string Md5(const std::string& str)
{
	unsigned char md[MD5_DIGEST_LENGTH];
	MD5(reinterpret_cast<const unsigned char*>(str.c_str()), str.size(), md);

	std::stringstream ss;
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md[i]);
	}
	return ss.str();
}

#pragma warning(default: 4996)

#else



#endif 