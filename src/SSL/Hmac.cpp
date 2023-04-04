#include "Hmac.h"
using namespace std;
Hmac::Hmac()
{
}

std::string Hmac::hmac_sha256(std::string key, std::string message)
{
	this->key = key;
	this->message = message;
	init();
	updata();
	final();
	return std::string();
}

void Hmac::init()
{	
	int len = key.size();
	if (len <= 64)
	{
		for (size_t i = len; i < 64; i++)
		{
			key += "0";
		}
	}
	else
	{
		Sha256 sha256_1;
		key = sha256_1.getsha256(key);
	}
	for (int i = 0; i < SHA256_BLOCKLEN; i++)
	{
		output.push_back(key[i] ^ INNER_PAD);
	}
}

void Hmac::updata()
{
	Sha256 sha256_2;
	//cout << output << endl;
	output = output + message;
	output = sha256_2.getsha256(output);
}

void Hmac::final()
{
	std::string keyo;
	for (int i = 0; i < SHA256_BLOCKLEN; i++)
	{
		keyo.push_back(key[i] ^ OUTER_PAD);
	}
	output = keyo + output;
	Sha256 sha256_3;
	output = sha256_3.getsha256(output);
	cout << output << endl;
}

