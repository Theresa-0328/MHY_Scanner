#pragma once
#include <string>

class ConfigDate final
{
public:
	static ConfigDate& getInstance();
	void updateConfig(const std::string& config);
	std::string getConfig()const;
	std::string defaultConfig();
private:
	ConfigDate();
	~ConfigDate();
	std::string m_config;
	ConfigDate(const ConfigDate& config) = delete;
	const ConfigDate& operator=(const ConfigDate& config) = delete;
	std::string loadConfig();
	std::string readConfigFile(const std::string& filePath);
	void createDefaultConfigFile(const std::string& filePath, const std::string& Config);
	const std::string& ConfigFilePath = "./Config/userinfo.json";
};