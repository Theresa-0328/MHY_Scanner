#include "ThreadStreamProcess.h"
#include "ThreadSacn.h"
#include "ThreadDownload.h"

ThreadStreamProcess::ThreadStreamProcess(QObject* parent)
	: QThread(parent)
{

}

ThreadStreamProcess::~ThreadStreamProcess()
{
	this->requestInterruption();
	this->wait();
}

void ThreadStreamProcess::biliInitStream(int uid, std::string access_key, std::string uName)
{
	LoginData = m.verify(uid, access_key);
	m.setUserName(uName);
}

void ThreadStreamProcess::stop()
{
	stopStream = true;
}

static int a = 0;

void ThreadStreamProcess::run()
{
	ThreadDownload td;
	td.downloadInit(url);
	td.start();
	ThreadSacn threadsacn;
	QThread::msleep(3000);
	stopStream = false;
	VideoProcessor vp;
	vp.OpenVideo("./Cache/output.flv");//错误判断
	int64_t latestTimestamp = av_gettime_relative();
	if (vp.avformatContext->streams[vp.index]->start_time != AV_NOPTS_VALUE)
	{
		int64_t streamTimestamp = av_rescale_q(vp.avformatContext->streams[vp.index]->start_time,
			vp.avformatContext->streams[vp.index]->time_base, { 1, AV_TIME_BASE });
		if (streamTimestamp > latestTimestamp)
		{
			latestTimestamp = streamTimestamp;
		}
	}
	av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
	int f = 0;
#ifdef _DEBUG
	cv::namedWindow("Video");
#endif // _DEBUG
	while (true)
	{
		//int64_t video_duration_sec = vp.avformatContext->duration / AV_TIME_BASE;
		//size_t timeInSeconds = av_q2d(vp.avstream->time_base);
		f++;
		if (stopStream)
		{
#ifdef _DEBUG
			cv::destroyWindow("Video");
#endif // _DEBUG
			break;
		}
		int op1 = vp.read(vp.avPacket);
		if (vp.avPacket->stream_index != vp.index)
		{
			continue;
		}
		int op2 = vp.SendPacket(vp.avPacket);
		if (op2 != 0)
		{
			av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
			QThread::msleep(300);
		}
		{
			vp.ReceiveFrame(vp.avframe);
			// 转换像素格式
			sws_scale(vp.swsCtx, vp.avframe->data, vp.avframe->linesize, 0,
				vp.avCodecContext->height, vp.pFrameBGR->data, vp.pFrameBGR->linesize);
			cv::Mat img(vp.avCodecContext->height, vp.avCodecContext->width, CV_8UC3, vp.pFrameBGR->data[0]);
			cv::Rect roi(0, 0, 1280, 720);
			cv::Mat crop_img = img(roi);
			//待优化：缩小扫描区域以提高速度和降低cpu占用。注意到有1280 1980和60帧 30帧
			if (!threadsacn.isRunning())
			{
				//threadsacn.setImg(crop_img);
//#ifdef _DEBUG
				//std::cout << "scan count "<< a++ << std::endl;
				imshow("Video", crop_img);
				cv::waitKey(2);
//#endif // _DEBUG
				//threadsacn.start();
			}
		}
		if (threadsacn.uqrcode.find("biz_key=bh3_cn") != std::string::npos)
		{
			int retcode = m.scanCheck(threadsacn.getTicket(), LoginData);
			emit loginSResults(retcode == 0);
			threadsacn.uqrcode.clear();
			break;
		}
		av_packet_unref(vp.avPacket);
	}
	td.stop();
}
