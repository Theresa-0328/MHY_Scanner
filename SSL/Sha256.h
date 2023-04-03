#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>

class Sha256
{
public:
	Sha256();
	Sha256(std::string data);
	std::string init();
//private:
	std::string data = {};
	std::string output = {};
	std::vector<unsigned int>m_data;
};

