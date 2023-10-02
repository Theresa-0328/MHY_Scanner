#pragma once

extern "C"
{
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libavutil/imgutils.h>
#include <libavutil/time.h>
#include <libswscale/swscale.h>
};
#include <QThread>
#include <QMutex>

#include "OfficialApi.h"
#include "Mihoyosdk.h"
#include "Common.h"

class ThreadStreamProcess : public QThread
{
	Q_OBJECT
public:
	ThreadStreamProcess(QObject* parent = nullptr);
	~ThreadStreamProcess();
	void setLoginInfo(const std::string& uid, const std::string& gameToken);
	void setLoginInfo(const std::string& uid, const std::string& gameToken, const std::string& name);
	void setServerType(const ServerType::Type servertype);
	void setUrl(const std::string& url, const std::map<std::string, std::string> heard = {});
	auto init() -> bool;
	void run();
	void stop();
signals:
	void loginResults(const ScanRet::Type ret);
private:
	OfficialApi o;
	Mihoyosdk m;
	std::string m_url;
	void LoginOfficial();
	void LoginBH3BiliBili();
	QMutex m_mux;
	std::string uid;
	std::string gameToken;
	std::string m_name;
	ServerType::Type servertype = ServerType::Type::UNKNOW;
	ScanRet::Type ret = ScanRet::Type::UNKNOW;
	AVDictionary* pAvdictionary;
	AVFormatContext* pAVFormatContext;
	AVCodecContext* pAVCodecContext;
	SwsContext* pSwsContext;
	AVFrame* pAVFrame;
	AVPacket* pAVPacket;
	int videoStremIndex = 0;
	bool m_stop = false;
};