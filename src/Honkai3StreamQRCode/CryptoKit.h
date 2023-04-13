#pragma once
#include <string>
class CryptoKit
{
public:
	static std::string rsaEncrypt(const std::string& message, const std::string& public_key);
	static std::string FormatRsaPublicKey(const std::string& key);
	static std::string HmacSha256(const std::string& message, const std::string& key);
	static std::string Md5(const std::string& str);
};