#include "ConfigDate.h"

#include <iostream>
#include <fstream>
#include <filesystem>

ConfigDate::ConfigDate()
{
    m_config = loadConfig();
}

ConfigDate::~ConfigDate()
{
}

ConfigDate& ConfigDate::getInstance()
{
    static ConfigDate instance;
    return instance;
}

void ConfigDate::updateConfig(const std::string& config)
{
    m_config = config;
    std::ofstream outFile(ConfigFilePath);
    outFile << config;
    outFile.close();
}

std::string ConfigDate::getConfig() const
{
    return m_config;
}

std::string ConfigDate::loadConfig()
{
    if (std::filesystem::exists(ConfigFilePath))
    {
        const std::string& configContent = readConfigFile(ConfigFilePath);
        return configContent;
    }
    else
    {
        return defaultConfig();
    }
}

std::string ConfigDate::readConfigFile(const std::string& filePath)
{
    std::ifstream file(filePath);
    if (file.is_open())
    {
        const std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        file.close();
        std::cout << "读取配置文件成功。\n";
        return content;
    }
    else
    {
        std::cout << "无法读取配置文件。\n";
        return "";
    }
}

void ConfigDate::createDefaultConfigFile(const std::string& filePath, const std::string& Config)
{
    std::filesystem::path directory = std::filesystem::path(filePath).parent_path();
    if (!std::filesystem::exists(directory))
    {
        if (!std::filesystem::create_directories(directory))
        {
            std::cout << "无法创建配置文件夹。\n";
            exit(0);
        }
    }
    std::ofstream file(filePath);
    if (file.is_open())
    {
        file << Config;
        file.close();
        std::cout << "创建并写入默认配置文件成功。\n";
    }
    else
    {
        std::cout << "无法创建配置文件。\n";
    }
}

std::string ConfigDate::defaultConfig()
{
    const static char* defaultConfig = R"({"auto_exit": false,"auto_login":false,"auto_start": false,"account":[],"last_account":0,"num":0})";
    m_config = defaultConfig;
    createDefaultConfigFile(ConfigFilePath, defaultConfig);
    return defaultConfig;
}
