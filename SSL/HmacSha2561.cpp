#include "HmacSha2561.h"

std::string HmacSha256::Hmac_Sha256(std::string key, std::string message)
{
    std::string outdata;
    std::string md;
    int keyLen = key.size();
    int messageLen = message.size();
    std::cout << keyLen << " " << messageLen << std::endl;
    return outdata;
}
