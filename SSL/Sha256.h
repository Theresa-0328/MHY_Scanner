#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

class Sha256
{
public:
	Sha256(std::string data);
//private:
	std::string data = {};
	std::string output = {};
};

