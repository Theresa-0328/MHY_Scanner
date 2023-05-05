#include "ThreadStreamProcess.h"
#include <QImage>

ThreadStreamProcess::ThreadStreamProcess(QObject *parent)
	: QThread(parent)
{

}

ThreadStreamProcess::~ThreadStreamProcess()
{
	cv::destroyAllWindows();
}

void ThreadStreamProcess::biliInitStream(int uid, std::string access_key, std::string uName)
{
	LoginData = m1.verify(uid, access_key);
	m1.setUserName(uName);
}

void ThreadStreamProcess::stop()
{
	stopStream = true;
}
static int a = 1;
void ThreadStreamProcess::run()
{
	QThread::msleep(3000);
	stopStream = false;
	VideoProcessor vp;
	vp.OpenVideo("./cache/output.flv");//错误判断
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
	QRScanner s;
	std::string qrCode;
	std::vector<cv::Mat> imageTemp;
	int f = 0;
	while (true)
	{
		f++;
		if (stopStream)
		{
			//cv::destroyWindow("Video");
			break;
		}
		int op1 = vp.read(vp.avPacket);
		if (vp.avPacket->stream_index != vp.index)
		{
			continue;
		}
		int op2 = vp.SendPacket(vp.avPacket);
		if (a % 5 == 0)
		{
			av_seek_frame(vp.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
		}
		while (true)
		{
			int op3 = vp.ReceiveFrame(vp.avframe);
			if (op3 != 0)
			{
				cv::waitKey(1000);
				break;
			}
			// 转换像素格式
			sws_scale(vp.swsCtx, vp.avframe->data, vp.avframe->linesize, 0,
				vp.avCodecContext->height, vp.pFrameBGR->data, vp.pFrameBGR->linesize);
			cv::Mat img(vp.avCodecContext->height, vp.avCodecContext->width, CV_8UC3, vp.pFrameBGR->data[0]);
			imageTemp.push_back(img);
			if (f > 30)
			{
				s.Decode(img, qrCode);
				//imshow("Video", imageTemp.back());
				//cv::waitKey(1);
				//std::cout << "测试命中" << a++ << std::endl;
				f = 0;
				imageTemp.clear();
			}
			break;
		}
		if (qrCode.find("biz_key=bh3_cn") != std::string::npos)
		{
			int retcode = m1.scanCheck(qrCode, LoginData);
			emit loginSResults(retcode == 0);
			break;
		}
		if (qrCode != "")
		{
			std::cout << "非崩坏3三二维码" << std::endl;
		}
		av_packet_unref(vp.avPacket);
	}
}
