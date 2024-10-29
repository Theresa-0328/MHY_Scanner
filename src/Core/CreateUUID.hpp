#pragma once

#include <string>
#include <string_view>
#include <random>
#include <array>

namespace CreateUUID
{
[[nodiscard]] inline std::string CreateUUID4()
{
    static constinit const std::string_view chars{ "0123456789abcdef" };
    static constinit const int UUID_LENGTH{ 36 };
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<int> distribution(0, 15);
    std::array<char, UUID_LENGTH + 1> uuid{};
    for (size_t i{}; i < UUID_LENGTH; ++i)
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
    return std::string(uuid.data());
}
}