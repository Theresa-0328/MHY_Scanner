#pragma once

class ServerType
{
public:
	enum Type
	{
		UNKNOW = 0,
		Official = 1,
		BH3_BiliBili = 2
	};
};

class GameType
{
public:
	enum Type
	{
		UNKNOW = 0,
		Honkai3 = 1,
		Genshin = 4,
		StarRail = 8
	};
};

class ScanRet
{
public:
	enum Type
	{
		UNKNOW = 0,
		SUCCESS = 1,
		FAILURE_1 = 3,
		FAILURE_2 = 4,
		LIVESTOP = 5
	};
};