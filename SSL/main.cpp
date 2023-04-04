//https://blog.csdn.net/Clnulijiayou/article/details/117374825#comments_25096682
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "HmacSha256.h"
#include"Sha256.h"
int main()
{	
	uint8_t outdata[128] = { 0 };
	uint8_t md[SHA256_DIGESTLEN] = { 0 };
	//ÃÜÔ¿
	uint8_t secret[] = { "0ebc517adb1b62c6b408df153331f9aa" };
	//Êý¾Ý
	uint8_t indata[] = { "app_id=1&channel_id=14&data={\"uid\":96023077,\"access_key\":\"ce03007dae8fec024b8482a08e8dda94_sh\"}&device=0000000000000000" };
	int len1 = strlen((char*)secret);
	int len2 = strlen((char*)indata);

	HMAC_SHA256_CTX hmac;
	hmac_sha256_init(&hmac, secret, len1);
	hmac_sha256_update(&hmac, indata, len2);
	hmac_sha256_final(&hmac, md);
	memcpy(outdata, md, SHA256_DIGESTLEN);

	for (int i = 0; i < SHA256_DIGESTLEN; i++)
	{
		printf("%02x", outdata[i]);
	}
	printf("\n");


	std::string s = "0ebc517adb1b62c6b408df153331f9aa";
	Sha256 sha;
	std::cout << sha.getsha256(s) << std::endl;
	return 0;
}
