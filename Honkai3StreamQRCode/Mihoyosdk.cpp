#include "Mihoyosdk.h"

std::string Mihoyosdk::verify(std::string uid, std::string access_key)
{
	std::cout << "verify with uid="<<uid << std::endl;
	json::Json data;
	data.parse(verifyData);
	data["uid"] = uid;
	data["access_key"] = access_key;
	json::Json body;
	body.parse(verifyBody);
	body["data"] = data;
	std::cout << body.str() << std::endl;
	return std::string();
}