#include <string.h>
#include <stdexcept>
#include <cstdlib>
#include "Parser.h"

json::Parser::Parser():m_idx(0)
{
}

void json::Parser::load(const std::string& str)
{
	m_str = str;
	m_idx = 0;
}

void json::Parser::skip_white_space()
{
	while (m_str[m_idx] == ' ' || m_str[m_idx] == '\n' || m_str[m_idx] == '\r' || m_str[m_idx] == '\t')
	{
		m_idx++;
	}
}

char json::Parser::get_next_token()
{
	skip_white_space();
	char ch = m_str[m_idx];
	m_idx++;
	return ch;
}

json::Json json::Parser::parse()
{
	char ch = get_next_token();
	switch (ch)
	{
	case'n':
		m_idx--;
		return parse_null();
	case't':
	case'f':
		m_idx--;
		return parse_bool();
	case'-':
	case'0':
	case'1':
	case'2':
	case'3':
	case'4':
	case'5':
	case'6':
	case'7':
	case'8':
	case'9':
		m_idx--;
		return parse_number();
	case'"':
		return Json(parse_string());
	case'[':
		return parse_array();
	case'{':
		return parse_object();
	default:
		break;
	}
	throw new std::logic_error("unexpected char");
}

json::Json json::Parser::parse_null()
{
	if (m_str.compare(m_idx, 4, "null") == 0)
	{
		m_idx += 4;
		return Json();
	}
	throw new std::logic_error("parse null error");
}

json::Json json::Parser::parse_bool()
{
	if (m_str.compare(m_idx, 4, "true") == 0)
	{
		m_idx += 4;
		return Json(true);
	}
	else if (m_str.compare(m_idx, 5, "false") == 0)
	{
		m_idx += 5;
		return Json(false);
	}
	throw new std::logic_error("parse bool error");
}

json::Json json::Parser::parse_number() 
{
	int pos = m_idx;
	if (m_str[m_idx] == '-')
	{
		m_idx++;
	}
	if (m_str[m_idx] < '0' || m_str[m_idx]>'9')
	{
		throw new std::logic_error("parse number error");
	}
	while(m_str[m_idx] >= '0' && m_str[m_idx] <= '9')
	{
		m_idx++;
	}
	if (m_str[m_idx] != '.')
	{
		int i = std::atoi(m_str.c_str() + pos);
		return Json(i);
	}
	m_idx++;
	if (m_str[m_idx] < '0' || m_str[m_idx]>'9')
	{
		throw new std::logic_error("parse number error");
	}
	while (m_str[m_idx] >= '0' && m_str[m_idx] <= '9')
	{
		m_idx++;
	}
	double f = std::atof(m_str.c_str() + pos);
	return Json(f);
}

json::Json json::Parser::parse_string()
{
	std::string out;
	while (true)
	{
		char ch = m_str[m_idx++];
		if (ch == '"')
		{
			break;
		}
		if (ch == '\\')
		{
			ch = m_str[m_idx++];
			switch (ch)
			{
			case'\n':
				out += '\n';
				break;
			case'\r':
				out += '\r';
				break;
			case'\t':
				out += '\t';
				break;
			case'\b':
				out += '\b';
				break;
			case'\f':
				out += '\f';
				break;
			case'"':
				out += "\\\"";
				break;
			case'\\':
				out += "\\\\";
				break;
			case'\\u':
				out += "\\u";
				for (int i = 0; i < 4; i++)
				{
					out += m_str[m_idx++];
				}
				break;
			default:
				break;
			}
		}
		else
		{
			out += ch;
		}
	}
	return out;
}

json::Json json::Parser::parse_array()
{
	Json arr(Json::json_array);
	char ch = get_next_token();
	if (ch == ']')
	{
		return arr;
	}
	m_idx--;
	while (true)
	{
		arr.append(parse());
		ch = get_next_token();
		if (ch == ']')
		{
			break;
		}
		if (ch != ',')
		{
			throw new std::logic_error("parse array error");
		}
		skip_white_space(); //BV1TP411p7cC 38:00
	}
	return arr;
}

json::Json json::Parser::parse_object()
{
	Json obj(Json::json_object);
	char ch = get_next_token();
	if (ch == '}')
	{
		return obj;
	}
	m_idx--;
	while (true)
	{
		ch = get_next_token();
		if (ch != '"')
		{
			throw new std::logic_error("parse object error");
		}
		std::string key = parse_string();
		ch = get_next_token();
		if (ch != ':')
		{
			throw new std::logic_error("parse object error");
		}
		obj[key] = parse();
		ch = get_next_token();
		if (ch == '}')
		{
			break;
		}
		if (ch != ',')
		{
			throw new std::logic_error("parse object error");
		}
		skip_white_space();
	}
	return obj;
}
