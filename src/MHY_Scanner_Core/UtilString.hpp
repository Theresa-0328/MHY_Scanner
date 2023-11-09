#pragma once

#include<string>

inline std::string escapeString(const std::string& input)
{
	std::string escaped;
	for (char c : input)
	{
		if (c == '\\')
		{
			escaped += "\\\\";
		}
		else if (c == '\"')
		{
			escaped += "\\\"";
		}
		else if (c == '\n')
		{
			escaped += "\\n";
		}
		else if (c == '\t')
		{
			escaped += "\\t";
		}
		else if (c == '\r')
		{
			escaped += "\\r";
		}
		else
		{
			escaped += c;
		}
	}
	return escaped;
}

inline std::string unescapeString(const std::string& input)
{
	std::string unescaped;
	bool isEscape = false;
	for (char c : input)
	{
		if (isEscape) {
			if (c == 'n')
			{
				unescaped += '\n';
			}
			else if (c == 't')
			{
				unescaped += '\t';
			}
			else if (c == 'r')
			{
				unescaped += '\r';
			}
			else
			{
				unescaped += c;
			}
			isEscape = false;
		}
		else
		{
			if (c == '\\')
			{
				isEscape = true;
			}
			else
			{
				unescaped += c;
			}
		}
	}
	return unescaped;
}

//temp
inline void replace0026WithAmpersand(std::string& str)
{
	std::string searchString = "0026";
	std::string replacement = "&";
	size_t pos = 0;

	while ((pos = str.find(searchString, pos)) != std::string::npos)
	{
		str.replace(pos, searchString.length(), replacement);
		pos += replacement.length();
	}
}