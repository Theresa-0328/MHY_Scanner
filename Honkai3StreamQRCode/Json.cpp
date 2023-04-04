#include <sstream>
#include <stdexcept>

#include "Json.h"
#include "Parser.h"

json::Json::Json():m_type(json_null)
{

}

json::Json::Json(bool value):m_type(json_bool)
{
	m_value.m_bool = value;
}

json::Json::Json(int value) :m_type(json_int)
{
	m_value.m_int = value;
}

json::Json::Json(double value) :m_type(json_double)
{
	m_value.m_double = value;
}

json::Json::Json(const char* value) :m_type(json_string)
{
	m_value.m_string = new std::string(value);
}

json::Json::Json(const std::string& value) :m_type(json_string)
{
	m_value.m_string = new std::string(value);
}

json::Json::Json(Type type) :m_type(type)
{
	switch (m_type)
	{
	case json::Json::json_null:
		break;
	case json::Json::json_bool:
		m_value.m_bool = false;
		break;
	case json::Json::json_int:
		m_value.m_int = 0;
		break;
	case json::Json::json_double:
		m_value.m_double = 0.0;
		break;
	case json::Json::json_string:
		m_value.m_string = new std::string("");
		break;
	case json::Json::json_array:
		m_value.m_array = new std::vector<Json>();
		break;
	case json::Json::json_object:
		m_value.m_object = new std::map<std::string, Json>();
		break;
	default:
		break;
	}
}

json::Json::Json(const Json& other)
{
	copy(other);
}

json::Json::operator bool()
{
	if (m_type != json_bool)
	{
		throw new std::logic_error("type_error,not bool value");
	}
	return m_value.m_bool;
}

json::Json::operator int()
{
	if (m_type != json_int)
	{
		throw new std::logic_error("type_error,not int value");
	}
	return m_value.m_int;
}

json::Json::operator double()
{
	if (m_type != json_double)
	{
		throw new std::logic_error("type_error,not double value");
	}
	return m_value.m_double;
}

json::Json::operator std::string()
{
	if (m_type != json_string)
	{
		throw new std::logic_error("type_error,not string value");
	}
	return *(m_value.m_string);
}

json::Json& json::Json::operator [] (int index)
{
	if (m_type != json_array)
	{
		m_type = json_array;
		m_value.m_array = new std::vector<Json>();
	}
	if (index < 0)
	{
		throw new std::logic_error("array[] index < 0");
	}
	int size = (int)(m_value.m_array)->size();
	if (index >= size) 
	{
		for (int i = size;i<=index;i++)
		{
			m_value.m_array->push_back(Json());
		}
	}
	return m_value.m_array->at(index);
}

json::Json & json::Json::operator[](const char* key)
{
	std::string name(key);
	return (*(this))[name];
}

json::Json& json::Json::operator [](const std::string& key)
{
	if (m_type != json_object)
	{
		clear();
		m_type = json_object;
		m_value.m_object = new std::map<std::string, Json>();
	}
	return (*(m_value.m_object))[key];
}

void json::Json::operator = (const json::Json& other)
{
	clear();
	copy(other);
}

bool json::Json::operator == (const json::Json& other)
{
	if (m_type != other.m_type)
	{
		return false;
	}
	switch (m_type)
	{
	case json::Json::json_null:
		return true;
		break;
	case json::Json::json_bool:
		return m_value.m_bool == other.m_value.m_bool;
		break;
	case json::Json::json_int:
		return m_value.m_int == other.m_value.m_int;
		break;
	case json::Json::json_double:
		return m_value.m_double == other.m_value.m_double;
		break;
	case json::Json::json_string:
		return *(m_value.m_string) == *(m_value.m_string);
		break;
	case json::Json::json_array:
		return m_value.m_array == m_value.m_array;//开摆！
		break;
	case json::Json::json_object:
		return m_value.m_object == m_value.m_object;
		break;
	default:
		break;
	}
	return false;
}

bool json::Json::operator != (const json::Json& other)
{
	return !((*this) == other);
}

void json::Json::copy(const Json& other)
{
	m_type = other.m_type;
	switch (m_type)
	{
	case json::Json::json_null:
		break;
	case json::Json::json_bool:
		m_value.m_bool = other.m_value.m_bool;
		break;
	case json::Json::json_int:
		m_value.m_int = other.m_value.m_int;
		break;
	case json::Json::json_double:
		m_value.m_double = other.m_value.m_double;
		break;
	case json::Json::json_string:
		m_value.m_string = other.m_value.m_string;
		break;
	case json::Json::json_array:
		m_value.m_array = other.m_value.m_array;
		break;
	case json::Json::json_object:
		m_value.m_object = other.m_value.m_object;
		break;
	default:
		break;
	}
}

void json::Json::clear()
{
	switch (m_type)
	{
	case json::Json::json_null:
		break;
	case json::Json::json_bool:
		m_value.m_bool = false;
		break;
	case json::Json::json_int:
		m_value.m_int = 0;
		break;
	case json::Json::json_double:
		m_value.m_double = 0;
		break;
	case json::Json::json_string:
		{
		delete m_value.m_string;
		break;
		}
	case json::Json::json_array:
		{
			for (auto it = (m_value.m_array)->begin(); it != (m_value.m_array)->end(); it++)
			{
				it->clear();
			}
		}
		delete m_value.m_array;
		break;
	case json::Json::json_object:
		{
			for (auto it = (m_value.m_object)->begin(); it != (m_value.m_object)->end(); it++)
			{
				(it->second).clear();
			}
			delete m_value.m_object;
			break;
		}
	default:
		break;
	}
	m_type = json_null;
}

void json::Json::append(const json::Json& other)
{
	if (m_type != json_array)
	{
		clear();
		m_type = json_array;
		m_value.m_array = new std::vector<Json>();
	}
	(m_value.m_array)->push_back(other);
}

std::string json::Json::str() const
{
	std::stringstream ss;
	switch (m_type)
	{
	case json::Json::json_null:
		ss << "null";
		break;
	case json::Json::json_bool:
		if (m_value.m_bool)
		{
			ss << "true";
		}
		else
		{
			ss << "false";
		}
		break;
	case json::Json::json_int:
		ss << m_value.m_int;
		break;
	case json::Json::json_double:
		ss << m_value.m_double;
		break;
	case json::Json::json_string:
		ss <<'\"' <<*(m_value.m_string)<< '\"';
		break;
	case json::Json::json_array:
	{
		ss << '[';
		for (auto it = (m_value.m_array)->begin(); it != (m_value.m_array)->end();it++)
		{
			if (it != (m_value.m_array)->begin())
			{
				ss << ',';
			}
			ss << it->str();
		}
		ss << ']';
		break;
	}
	case json::Json::json_object:
	{
		ss << '{';
		for (auto it = (m_value.m_object)->begin(); it != (m_value.m_object)->end(); it++)
		{
			if (it != (m_value.m_object)->begin())
			{
				ss << ',';
			}
			ss << '\"'<<it->first << '\"'<<':'<< it->second.str();
		}
		ss << '}';
		break;
	}
	default:
		break;
	}
	return ss.str();
}

bool json::Json::asBool() const
{
	if (m_type != json_bool)
	{
		throw new std::logic_error("type error,not bool value");
	}
	return m_value.m_bool;
}

int json::Json::asInt() const
{
	if (m_type != json_int)
	{
		throw new std::logic_error("type error,not int value");
	}
	return m_value.m_int;
}

double json::Json::asDouble() const
{
	if (m_type != json_double)
	{
		throw new std::logic_error("type error,not int value");
	}
	return m_value.m_int;
}

std::string json::Json::asString() const
{
	if (m_type != json_string)
	{
		throw new std::logic_error("type error,not string value");
	}
	return *(m_value.m_string);
}

bool json::Json::sha256_transform(int index)
{
	if (m_type != json_array)
	{
		return false;
	}
	int size = (int)(m_value.m_array)->size();
	return (index >= 0 && index < size);
}

bool json::Json::sha256_transform(const char* key)
{
	std::string name(key);
	return sha256_transform(name);
}

bool json::Json::sha256_transform(const std::string key)
{
	if (m_type != json_object)
	{
		return false;
	}
	return ((m_value.m_object)->find(key) != m_value.m_object->end());
}

void json::Json::remove(int index)
{
	if (m_type != json_array)
	{
		return ;
	}
	int size = (int)(m_value.m_array)->size();
	if (index < 0 || index >= size)
	{
		return;
	}
	(m_value.m_array)->at(index).clear();
	(m_value.m_array)->erase((m_value.m_array->begin()) + index);
}

void json::Json::remove(const char* key)
{
	std::string name(key);
	remove(name);
}

void json::Json::remove(const std::string& key)
{
	auto it = m_value.m_object->find(key);
	if (it == m_value.m_object->end())
	{
		return;
	}
	(*(m_value.m_object))[key].clear();
	(m_value.m_object)->erase(key);
}

void json::Json::parse(const std::string str)
{
	Parser p;
	p.load(str);
	*this = p.parse();
}

std::map<std::string, std::string> json::Json::objToMap()
{
	std::map<std::string, std::string> re;
	if (m_value.m_object != nullptr) 
	{
		for (auto it = m_value.m_object->begin(); it != m_value.m_object->end(); it++) 
		{
			re[it->first] = it->second.str();
		}
	}
	return re;
}
