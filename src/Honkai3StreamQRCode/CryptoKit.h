#pragma once
#include <string>
class CryptoKit
{
public:
	static std::string rsaEncrypt(std::string message, std::string public_key);
	static std::string formatRsaPublicKey(const std::string& key);
};