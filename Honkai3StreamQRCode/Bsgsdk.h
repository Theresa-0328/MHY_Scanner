#pragma once
#include <algorithm>
#include <map>

#include "Json.h"
#include "Parser.h"
#include "..//SSL/Md5.h"
#include "utils.h"

class Bsgsdk
{
public:
	//Bsgsdk();
	//~Bsgsdk();
	std::string setSign(std::map<std::string, std::string>data);
	json::Json getUserInfo(std::string, std::string);//��¼�����˺�
	//json::Json login();//��¼�˺�
//private:
	std::string remove_quotes(std::string str);
	const std::string bililogin = "https://line1-sdk-center-login-sh.biligame.net/";
	//json::Json a;
	utils u;
	const std::string userinfoParam = "{\"cur_buvid\":\"XZA2FA4AC240F665E2F27F603ABF98C615C29\",\"client_timestamp\":1667057013442,\"sdk_type\":1,\"isRoot\":0,\"merchant_id\":\"590\",\"dp\":\"1280*720\",\"mac\":\"08:00:27:53:DD:12\",\"uid\":437470182,\"support_abis\":\"x86,armeabi-v7a, armeabi\",\"apk_sign\":\"4502a02a00395dec05a4134ad593224d\",\"platform_type\":\"3\",\"old_buvid\":\"XZA2FA4AC240F665E2F27F603ABF98C615C29\",\"operators\":\"5\",\"fingerprint\":\"\",\"model\":\"MuMu\",\"udid\":\"XXA31CBAB6CBA63E432E087B58411A213BFB7\",\"net\":\"5\",\"app_id\":\"180\",\"brand\":\"Android\",\"oaid\":\"\",\"game_id\":\"180\",\"timestamp\":\"1667057013275\",\"ver\":\"6.1.0\",\"c\":\"1\",\"version_code\":\"510\",\"server_id\":\"378\",\"version\":\"1\",\"domain_switch_count\":\"0\",\"pf_ver\":\"12\",\"access_key\":\"\",\"domain\":\"line1-sdk-center-login-sh.biligame.net\",\"original_domain\":\"\",\"imei\":\"\",\"sdk_log_type\":\"1\",\"sdk_ver\":\"3.4.2\",\"android_id\":\"84567e2dda72d1d4\",\"channel_id\":1}";
};