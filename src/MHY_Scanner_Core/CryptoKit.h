#pragma once

#include <string>

class CryptoKit
{
public:
	std::string rsaEncrypt(const std::string& message, const std::string& public_key);
	void FormatRsaPublicKey(std::string& key);
	std::string HmacSha256(const std::string& message, const std::string& key);
	void Md5(std::string& str);
};