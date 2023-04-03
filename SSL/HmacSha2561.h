#pragma once
#include<string>
#include<iostream>

#define SHA256_BLOCKLEN  64ul //size of message block buffer
#define SHA256_DIGESTLEN 32ul //size of digest in uint8_t
#define SHA256_DIGESTINT 8ul  //size of digest in uint32_t

class HmacSha256
{
public:
	std::string Hmac_Sha256(std::string key, std::string message);
	typedef struct sha256_ctx_t
	{
		uint64_t len;                 // processed message length
		uint32_t h[SHA256_DIGESTINT]; // hash state
		uint8_t buf[SHA256_BLOCKLEN]; // message block buffer
	} SHA256_CTX;
};

