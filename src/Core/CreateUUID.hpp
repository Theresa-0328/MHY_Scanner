#pragma once

#include <string>
#include <random>

namespace CreateUUID
{
[[nodiscard]] inline std::string CreateUUID4()
{
    static const char chars[] = "0123456789abcdef";
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 15);
    static constinit const int UUID_LENGTH{ 36 };
    char uuid[UUID_LENGTH + 1]{};
    for (int i{}; i < UUID_LENGTH; ++i)
    {
        if (i == 8 || i == 13 || i == 18 || i == 23)
        {
            uuid[i] = '-';
            continue;
        }
        const int randomDigit{ distribution(generator) };
        uuid[i] = chars[randomDigit];
    }
    uuid[UUID_LENGTH] = '\0';
    return std::string(uuid);
}
}