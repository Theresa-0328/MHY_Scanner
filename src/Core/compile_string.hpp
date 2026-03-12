#include <array>
#include <cstddef>
#include <algorithm>
#include <string_view>

template<std::size_t N>
class compile_string
{
public:
    std::array<char, N> data{};

    constexpr compile_string(const char (&s)[N])
    {
        std::copy_n(s, N, data.begin());
    }
    constexpr compile_string(std::array<char, N> arr) :
        data(arr) {}

    constexpr const char* c_str() const { return data.data(); }
    constexpr std::size_t size() const { return N - 1; }

    constexpr operator std::string_view() const { return { data.data(), N - 1 }; }
    explicit operator std::string() const { return { data.data(), N - 1 }; }

    template<std::size_t M>
    constexpr auto operator+(const compile_string<M>& rhs) const
    {
        std::array<char, N + M - 1> result{};
        std::copy_n(data.begin(), N - 1, result.begin());
        std::copy_n(rhs.data.begin(), M, result.begin() + N - 1);
        return compile_string<N + M - 1>(result);
    }
};