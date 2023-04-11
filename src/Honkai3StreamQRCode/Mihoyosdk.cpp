#include "Mihoyosdk.h"
#include "HmacSha256.hpp"
#include "Json.h"
#include "Parser.h"
std::string Mihoyosdk::verify(const int uid, const std::string access_key)
{
	std::cout << "verify with uid=" << uid << std::endl;
	verifyData["uid"] = std::to_string(uid);
	verifyData["access_key"] = access_key;
	std::string ssd = "{\\\"access_key\\\":\\\"" + verifyData["access_key"] + "\\\""",\\\"uid\\\":" + verifyData["uid"] + "}";
	json::Json body;
	body.parse(verifyBody);
	body["data"] = ssd;
	const std::string sBody = body.str();
	body.clear();
	std::string s;
	u.PostRequest(s, loginV2Url, makeSign(sBody));
	std::cout << "崩坏3验证完成，登录成功" << std::endl;
	return s;
}

std::string Mihoyosdk::getBHVer()
{
	return "6.5.0";
}

std::string Mihoyosdk::getOAServer()
{

	std::string bhVer = getBHVer();
	std::string oaMainUrl = "https://global2.bh3.com/query_dispatch?";
	std::string param = "version=" + bhVer + "_gf_android_bilibili&t=" + std::to_string(u.getCurrentUnixTime());
	std::string feedback;
	u.PostRequest(feedback, oaMainUrl + param, "");
	//std::cout << u.UTF8_To_string(feedback) << std::endl;
	param = "?version=" + bhVer + "_gf_android_bilibili&t=" + std::to_string(u.getCurrentUnixTime());
	json::Json j;
	j.parse(feedback);
	std::string dispatch_url = j["region_list"][0]["dispatch_url"];
	std::string dispatch;
	u.PostRequest(dispatch, dispatch_url + param, "");
	dispatch = u.UTF8_To_string(dispatch);
	std::cout << "获得OA服务器成功 : " << dispatch << std::endl;
	j.clear();
	return dispatch;
}

void Mihoyosdk::scanCheck(const std::string& qrCode, const std::string& bhInfo)
{
	int pos = (int)qrCode.find("ticket=");
	std::string ticket;
	int i = (int)qrCode.size();
	if (pos != std::string::npos)
	{
		ticket = qrCode.substr(pos + 7);
	}
	json::Json check;
	check.parse(scanCheckS);
	check["ticket"] = ticket;
	check["ts"] = u.getCurrentUnixTime();
	std::string postBody = makeSign(check.str());
	std::string feedback;
	u.PostRequest(feedback, "https://api-sdk.mihoyo.com/bh3_cn/combo/panda/qrcode/scan", postBody);
	check.parse(feedback);
	if ((int)check["retcode"] == 0)
	//if ((int)check["retcode"] != 0)
	{
		std::cout << "扫码失败" << std::endl;
		std::cout << feedback << std::endl;
		return;
	}
	else
	{
		scanConfirm(ticket, bhInfo);
	}
	check.clear();
}

void Mihoyosdk::scanConfirm(const std::string& ticket, const std::string& bhInfoR)
{
	json::Json bhInfoJ;
	bhInfoJ.parse(bhInfoR);
	
	json::Json bhInFo;
	bhInFo.parse(bhInfoJ["data"].str());
	//std::cout << bhInFo.str() << std::endl;
	json::Json scanResultJ;
	scanResultJ.parse(scanResult);
	
	json::Json scanDataJ;
	scanDataJ.parse(scanData);
	
	json::Json oa;
	oa.parse(getOAServer());
	scanDataJ["dispatch"] = oa;
	scanDataJ["accountID"] = bhInFo["open_id"];
	scanDataJ["accountToken"] = bhInFo["combo_token"];
	//std::cout << scanDataJ.str() << std::endl;
	json::Json scanExtJ;
	scanExtJ.parse(scanExtR);
	//std::cout << scanExtJ.str() << std::endl;
	scanExtJ["data"] = scanDataJ;

	json::Json scanRawJ;
	scanRawJ.parse(scanRawR);
	scanRawJ["open_id"] = bhInFo["open_id"];
	scanRawJ["combo_id"] = bhInFo["combo_id"];
	scanRawJ["combo_token"] = bhInFo["combo_token"];
	scanRawJ["asterisk_name"] = u.string_To_UTF8("测试中文名称testname1");
	json::Json scanPayLoadJ;
	scanPayLoadJ.parse(scanPayloadR);
	scanPayLoadJ["raw"] = scanRawJ;
	scanPayLoadJ["ext"] = scanExtJ;
	scanResultJ["payload"] = scanPayLoadJ;
	scanResultJ["ts"] = u.getCurrentUnixTime();
	scanResultJ["ticket"] = ticket;
	std::string postBody = scanResultJ.str();
	postBody = makeSign(postBody);
	json::Json postBodyJ;
	postBodyJ.parse(postBody);
	std::string a1 = postBodyJ["payload"]["ext"].str();
	a1 = u.replaceQuotes(a1);
	postBodyJ["payload"]["ext"] = a1;
	std::string a2 = postBodyJ["payload"]["raw"].str();
	a2 = u.replaceQuotes(a2);
	postBodyJ["payload"]["raw"] = a2;
	postBody = postBodyJ.str();
	std::cout << postBody<< std::endl;
	std::string response;
	u.PostRequest(response, "https://api-sdk.mihoyo.com/bh3_cn/combo/panda/qrcode/confirm", postBody);
	postBodyJ.parse(response);
	if ((int)postBodyJ["retcode"] == 0)
		std::cout << "扫码成功" << std::endl;
	else
		std::cout << "扫码失败" << std::endl;
		std::cout << postBodyJ.str() << std::endl;
	return;
}

std::string Mihoyosdk::makeSign(const std::string data)
{
	std::string sign;
	std::string	data2;
	json::Json p;
	p.parse(data);
	std::map<std::string, std::string> m = p.objToMap();
	for (auto& it : m)
	{
		if (it.first == "sign")
			continue;
		if (it.first == "data")
		{
			if (it.second.front() == '\"')
				it.second.erase(0, 1);
			if (it.second.back() == '\"')
				it.second.pop_back();
		}
		if (it.first == "device")
		{
			if (it.second.front() == '\"')
				it.second.erase(0, 1);
			if (it.second.back() == '\"')
				it.second.pop_back();
		}
		data2 += it.first + "=" + it.second + "&";
	}
	data2.erase(data2.length() - 1);
	std::cout << "data2=" << data2 << std::endl;
	sign = bh3Sign(data2);
	p["sign"] = sign;
	sign = p.str();
	p.clear();
	return sign;
}

std::string Mihoyosdk::bh3Sign(std::string data)
{
	std::cout << data << std::endl;
	std::string key = "0ebc517adb1b62c6b408df153331f9aa";
	data.erase(std::remove(data.begin(), data.end(), '\\'), data.end());
	std::cout << data << std::endl;
	std::string sign = hmac_sha256(key, data);
	std::cout << "Hmac_Sha256签名完成" << std::endl;
	return sign;
}
