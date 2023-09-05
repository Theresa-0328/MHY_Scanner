#pragma once

#include<string>

std::string escapeString(const std::string& input)
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

std::string unescapeString(const std::string& input)
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