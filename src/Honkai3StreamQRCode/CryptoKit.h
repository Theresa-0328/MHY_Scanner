#pragma once
#include <string>
class CryptoKit
{
public:
	static std::string rsaEncrypt(const std::string& message, const std::string& public_key);
	static void FormatRsaPublicKey(std::string& key);
	static std::string HmacSha256(const std::string& message, const std::string& key);
	static void Md5(std::string& str);
};