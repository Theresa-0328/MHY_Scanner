#include "ThreadStreamProcess.h"
#include "ThreadSacn.h"
#include "ThreadDownload.h"
#include "OfficialApi.h"
#include "Mihoyosdk.h"

ThreadStreamProcess::ThreadStreamProcess(QObject* parent)
	: QThread(parent)
{

}

ThreadStreamProcess::~ThreadStreamProcess()
{
	if (!this->isInterruptionRequested())
	{
		QMutexLocker lock(&m_mux);
		stopStream = true;
	}
	this->requestInterruption();
	this->wait();
}

void ThreadStreamProcess::Init0(std::string uid, std::string gameToken)
{
	this->uid = uid;
	this->gameToken = gameToken;
}

void ThreadStreamProcess::Init1(const std::string uid, const std::string gameToken, const std::string uName)
{
	this->uid = uid;
	this->gameToken = gameToken;
	this->uName = uName;
}


void ThreadStreamProcess::serverType0()
{
	ThreadDownload td;
	td.downloadInit(url);
	td.start();
	OfficialApi o;
	ThreadSacn threadsacn;
	QThread::msleep(3000);
	VideoProcessor vp;
	//错误判断
	int onum = vp.OpenVideo("./Cache/output.flv");
	assert(onum==0);
	int64_t latestTimestamp = av_gettime_relative();
	
	//if (vp.avformatContext->streams[vp.index]->start_time != AV_NOPTS_VALUE)
	//{
	//	int64_t streamTimestamp = av_rescale_q(vp.avformatContext->streams[vp.index]->start_time,
	//		vp.avformatContext->streams[vp.index]->time_base, { 1, AV_TIME_BASE });
	//	if (streamTimestamp > latestTimestamp)
	//	{
	//		latestTimestamp = streamTimestamp;
	//	}
	//}

	av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
	int f = 0;
#ifdef _DEBUG
	cv::namedWindow("Video");
#endif // _DEBUG
	while (true)
	{
		//int64_t video_duration_sec = vp.avformatContext->duration / AV_TIME_BASE;
		//size_t timeInSeconds = av_q2d(vp.avstream->time_base);
		if (stopStream)
		{
#ifdef _DEBUG
			cv::destroyWindow("Video");
#endif // _DEBUG
			break;
		}
		int op1 = vp.read();
		if (vp.avPacket->stream_index != vp.index)
		{
			continue;
		}
		if (vp.SendPacket() != 0)
		{
			av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
			QThread::msleep(300);
		}
		{
			vp.ReceiveFrame();
			// 转换像素格式
			sws_scale(vp.swsCtx, vp.avframe->data, vp.avframe->linesize, 0,
				vp.avCodecContext->height, vp.pFrameBGR->data, vp.pFrameBGR->linesize);
			cv::Mat img(vp.avCodecContext->height, vp.avCodecContext->width, CV_8UC3, vp.pFrameBGR->data[0]);
			//可能有1280 1980和60帧 30帧,二维码不一定在屏幕的正中央。
			//cv::Mat crop_img = img(cv::Range(300, 800), cv::Range(500, 1186));
			cv::Mat _img = img(cv::Rect(0, 0, 1280, 720));
			if (!threadsacn.isRunning())
			{
				threadsacn.setImg(_img);
#ifdef _DEBUG
				std::cout << "scan count "<< f++ << std::endl;
				imshow("Video", _img);
				cv::waitKey(1);
#endif
				threadsacn.start();
			}
		}
		if (threadsacn.uqrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			o.gameType = 1;
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken);
			emit loginResults(code == 0);
			continue;
		}
		//if (threadsacn.uqrcode.find("biz_key=hkrpg_cn") != std::string::npos)
		//{

		//}
		if (threadsacn.uqrcode.find("biz_key=hkrpg_cn") != std::string::npos)
		{
			o.gameType = 8;
			int code = o.scanRequest(threadsacn.getTicket(), uid, gameToken);
			emit loginResults(code == 0);
			continue;
		}
		av_packet_unref(vp.avPacket);
	}
	td.stop();
}

void ThreadStreamProcess::serverType1()
{
	ThreadDownload td;
	td.downloadInit(url);
	td.start();
	Mihoyosdk m;
	m.setOAServer();
	std::string LoginData;
	LoginData = m.verify(std::stoi(uid), gameToken);
	m.setUserName(uName);
	ThreadSacn threadsacn;
	QThread::msleep(3000);
	VideoProcessor vp;
	//错误判断
	if (vp.OpenVideo("./Cache/output.flv") != 0)
	{

	}
	int64_t latestTimestamp = av_gettime_relative();
	av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
	int f = 0;
#ifdef _DEBUG
	cv::namedWindow("Video");
#endif // _DEBUG
	while (true)
	{
		if (stopStream)
		{
#ifdef _DEBUG
			cv::destroyWindow("Video");
#endif // _DEBUG
			break;
		}
		int op1 = vp.read();
		if (vp.avPacket->stream_index != vp.index)
		{
			continue;
		}
		if (vp.SendPacket() != 0)
		{
			av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
			QThread::msleep(300);
		}
		{
			vp.ReceiveFrame();
			// 转换像素格式
			sws_scale(vp.swsCtx, vp.avframe->data, vp.avframe->linesize, 0,
				vp.avCodecContext->height, vp.pFrameBGR->data, vp.pFrameBGR->linesize);
			cv::Mat img(vp.avCodecContext->height, vp.avCodecContext->width, CV_8UC3, vp.pFrameBGR->data[0]);
			if (!threadsacn.isRunning())
			{
				threadsacn.setImg(img);
#ifdef _DEBUG
				std::cout << "scan count " << f++ << std::endl;
				imshow("Video", img);
				cv::waitKey(2);
#endif
				threadsacn.start();
			}
		}
		if (threadsacn.uqrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			int code = m.scanCheck(threadsacn.getTicket(), LoginData);
			emit loginResults(code == 0);
			continue;
		}
		av_packet_unref(vp.avPacket);
	}
	td.stop();
}

void ThreadStreamProcess::stop()
{
	stopStream = true;
}

void ThreadStreamProcess::run()
{
	stopStream = false;
	if (serverType == 0)
	{
		serverType0();
		return;
	}
	if (serverType == 1)
	{
		serverType1();
		return;
	}
}