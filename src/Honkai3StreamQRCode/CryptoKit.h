#pragma once
#include <string>
class CryptoKit
{
public:
	std::string rsaEncrypt(std::string message, std::string public_key);
	std::string formatRsaPublicKey(const std::string& key);
};
