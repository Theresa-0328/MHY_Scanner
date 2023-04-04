#pragma once
#include <string>
#include "Sha256.h"

#define INNER_PAD '\x36'
#define OUTER_PAD '\x5c'
class Hmac
{
public:
	Hmac();
	std::string hmac_sha256(std::string key, std::string message);
	void init();
	void updata();
	void final();
private:
	std::string key;
	std::string message;
	std::string output;
};