#include "main.h"
int main(int argc, char* argv[])
{
	v2api v2api;
	std::string playurl = v2api.Initialize();
	
	Download down;
	down.curlDownlod(playurl);
	
	system("pause");
	//std::ifstream fin("./config.json");
	//std::stringstream ss;
	//ss << fin.rdbuf();
	//const std::string& str = ss.str();
	//json::Json v;
	//v.parse(str);
	//json::Json a;
	//a = v["quality_description"][0];
	//std::cout << a.str() <<std::endl;
	////注意！！！！！v.clear();
	
	return 0;
}