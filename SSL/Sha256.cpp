#include "Sha256.h"

Sha256::Sha256()
{
	uint32_t k[] = {0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
					0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
					0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
					0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
					0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
					0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
					0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
					0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2};
	uint32_t h[] = {0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19};
	uint32_t R(uint32_t x, int p);
	uint32_t S(uint32_t x, int p);
	uint32_t z1(uint32_t x);
	uint32_t z0(uint32_t x);
	uint32_t ma(uint32_t x, uint32_t y, uint32_t z);
	uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
	uint32_t s1(uint32_t x);
	uint32_t s0(uint32_t x);
}

Sha256::~Sha256()
{

}

void print(uint32_t ans) {
    std::vector<char>res;
    while (ans) 
    {// Binary to Hex
        int now = ans % 16;
        char x;
        if (now < 10) x = now + '0';
        else x = now - 10 + 'a';
        ans /= 16;
        res.push_back(x);
    }
    while (res.size() < 8) res.push_back('0');// 32-bits is 8 nums of Hex
    reverse(res.begin(), res.end());
    for (auto x : res) std::cout << x;
}

void Sha256::sha256_transform()
{
    uint32_t x = 0;
    int p = 0;
    for (int i = 0; i < 512; ++i) { // Get the 16 nums of 32-bits
        x <<= 1;
        x += Data[i];
        if ((i + 1) % 32 == 0) {
            w[p++] = x;
            x = 0;
        }
    }
    for (int i = 16; i < 64; ++i) // Extension 64 nums of 32-bits
        w[i] = s1(w[i - 2]) + w[i - 7] + s0(w[i - 15]) + w[i - 16];
    uint32_t a[8];
    for (int i = 0; i < 8; ++i) a[i] = h[i]; // Init h[0] - h[7] 
    for (int i = 0; i < 64; ++i) {
        uint32_t sum1 = k[i] + w[i] + ch(a[4], a[5], a[6]) + z1(a[4]) + a[7];
        uint32_t sum2 = z0(a[0]) + ma(a[0], a[1], a[2]);
        for (int j = 7; j >= 0; --j) {// Except the fifth and first, a[j](new) = a[j - 1]
            if (j == 4) a[j] = a[j - 1] + sum1;
            else if (j == 0) a[j] = sum1 + sum2;
            else a[j] = a[j - 1];
        }
    }
    for (int i = 0; i < 8; ++i) {
        h[i] += a[i];// Ger h[i](new)
    }
}

void Sha256::sha256_update(const std::string str)
{
    std::vector<uint8_t> vec(str.begin(), str.end());
    char c;
    int sz = 0;
    for (auto it = vec.begin(); it != vec.end(); it++) {// When file is not EOF, we getchar.
        c = *it;
        sz += 8;// 1 char is 8 bits
        std::vector<bool> now;// This char binary
        int x = c;
        while (x) {
            if (x & 1) now.push_back(1);
            else now.push_back(0);
            x >>= 1;
        }
        while (now.size() < 8) now.push_back(0);// Added 0
        reverse(now.begin(), now.end());
        for (auto x : now) Data.push_back(x);
        if (Data.size() == 512) {// When 512bits
            sha256_transform();
            Data.clear();
        }
    }
    int cnt = Data.size();// Cnt is number of needed
    if (cnt >= 448) cnt = 448 + 512 - cnt;
    else cnt = 448 - cnt;
    Data.push_back(1);// Added 1
    --cnt;
    if (Data.size() == 512) {// When 512bits
        sha256_transform();
        Data.clear();
    }
    while (cnt) {// Added 0
        Data.push_back(0), --cnt;
        if (Data.size() == 512) {
            sha256_transform();
            Data.clear();
        }
    }
    // This section is added last 64bits
    std::vector<bool> now;
    while (sz) {// This 64bits is size of string 
        if (sz & 1) now.push_back(1);
        else now.push_back(0);
        sz >>= 1;
    }
    while (now.size() < 64) now.push_back(0);
    reverse(now.begin(), now.end());
    for (auto x : now) Data.push_back(x);
    sha256_transform();
    Data.clear();


    for (int i = 0; i < 8; ++i) {// Print sha256_transformh value
        print(h[i]);
    }
}

uint32_t Sha256::R(uint32_t x, int p)
{
	return (x >> p);
}

uint32_t Sha256::S(uint32_t x, int p)
{
	return (x >> p | x << 32 - p);
}

uint32_t Sha256::z1(uint32_t x)
{
	return S(x, 6) ^ S(x, 11) ^ S(x, 25);
}

uint32_t Sha256::z0(uint32_t x)
{
	return S(x, 2) ^ S(x, 13) ^ S(x, 22);
}

uint32_t Sha256::ma(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ (x & z) ^ (y & z);
}

uint32_t Sha256::ch(uint32_t x, uint32_t y, uint32_t z)
{
	return (x & y) ^ (~x & z);
}

uint32_t Sha256::s1(uint32_t x) {
	return S(x, 17) ^ S(x, 19) ^ R(x, 10);
}

uint32_t Sha256::s0(uint32_t x) {
	return S(x, 7) ^ S(x, 18) ^ R(x, 3);
}