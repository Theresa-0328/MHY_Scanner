#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <Windows.h>
#define SHA256_BLOCKLEN  64ul //size of message block buffer
#define SHA256_DIGESTLEN 32ul //size of digest in uint8_t
#define SHA256_DIGESTINT 8ul  //size of digest in uint32_t

class Sha256
{
public:
	Sha256();
	~Sha256();
	void sha256_update(const std::string m);
//private:
	uint32_t len = 0;
	uint8_t buf[SHA256_BLOCKLEN] = {};
	void sha256_transform();
	std::vector<bool> Data;
	uint32_t k[64];
	uint32_t h[8];
	uint32_t w[100];
	uint32_t R(uint32_t x, int p);
	uint32_t S(uint32_t x, int p);
	uint32_t z1(uint32_t x);
	uint32_t z0(uint32_t x);
	uint32_t ma(uint32_t x, uint32_t y, uint32_t z);
	uint32_t ch(uint32_t x, uint32_t y, uint32_t z);
	uint32_t s1(uint32_t x);
	uint32_t s0(uint32_t x);
};