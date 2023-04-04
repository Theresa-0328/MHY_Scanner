#include"Sha256.h"

using namespace std;
Sha256::Sha256()
{
    vector<bool> Data;
    uint32_t w[100];
    uint32_t R(uint32_t x, int p);
    uint32_t S(uint32_t x, int p);
    uint32_t z1(uint32_t x);
    uint32_t z0(uint32_t x);
    uint32_t ma(uint32_t x, uint32_t y, uint32_t z);
    uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
    uint32_t s1(uint32_t x);
    uint32_t s0(uint32_t x);
}

uint32_t Sha256::R(uint32_t x, int p) {
    return (x >> p);
}
uint32_t Sha256::S(uint32_t x, int p) {
    return (x >> p | x << 32 - p);
}
uint32_t Sha256::z1(uint32_t x) {
    return S(x, 6) ^ S(x, 11) ^ S(x, 25);
}
uint32_t Sha256::z0(uint32_t x) {
    return S(x, 2) ^ S(x, 13) ^ S(x, 22);
}
uint32_t Sha256::ma(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (x & z) ^ (y & z);
}
uint32_t Sha256::ch(uint32_t x, uint32_t y, uint32_t z) {
    return (x & y) ^ (~x & z);
}
uint32_t Sha256::s1(uint32_t x) {
    return S(x, 17) ^ S(x, 19) ^ R(x, 10);
}
uint32_t Sha256::s0(uint32_t x) {
    return S(x, 7) ^ S(x, 18) ^ R(x, 3);
}
void Sha256::print(uint32_t ans) {
    vector<char>res;
    while (ans) {// Binary to Hex
        int now = ans % 16;
        char x;
        if (now < 10) x = now + '0';
        else x = now - 10 + 'a';
        ans /= 16;
        res.push_back(x);
    }
    while (res.size() < 8) res.push_back('0');// 32-bits is 8 nums of Hex
    reverse(res.begin(), res.end());
    for (auto x : res) cout << x;
}
void Sha256::has() 
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
void Sha256::sha256_update(string m)
{
    std::vector<uint8_t> vec(m.begin(), m.end());
    char c;
    int sz = 0;
    for (auto it = vec.begin(); it != vec.end(); it++) {// When file is not EOF, we getchar.
        c = *it;
        sz += 8;// 1 char is 8 bits
        vector<bool> now;// This char binary
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
            has();
            Data.clear();
        }
    }
    int cnt = Data.size();// Cnt is number of needed
    if (cnt >= 448) cnt = 448 + 512 - cnt;
    else cnt = 448 - cnt;
    Data.push_back(1);// Added 1
    --cnt;
    if (Data.size() == 512) {// When 512bits
        has();
        Data.clear();
    }
    while (cnt) {// Added 0
        Data.push_back(0), --cnt;
        if (Data.size() == 512) {
            has();
            Data.clear();
        }
    }
    // This section is added last 64bits
    vector<bool> now;
    while (sz) {// This 64bits is size of string 
        if (sz & 1) now.push_back(1);
        else now.push_back(0);
        sz >>= 1;
    }
    while (now.size() < 64) now.push_back(0);
    reverse(now.begin(), now.end());
    for (auto x : now) Data.push_back(x);
    has();
    Data.clear();


    for (int i = 0; i < 8; ++i) {// Print hash value
        print(h[i]);
    }
}