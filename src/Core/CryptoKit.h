#pragma once

#include <string>

std::string rsaEncrypt(const std::string& message, const std::string& public_key);
std::string HmacSha256(const std::string& message, const std::string& key);
std::string Md5(const std::string& str);