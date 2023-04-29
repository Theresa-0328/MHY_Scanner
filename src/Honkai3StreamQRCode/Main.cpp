#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <opencv2/opencv.hpp>
#include <Windows.h>
#include <future>
#include <fstream>

#include "V2api.h"
#include "Download.h"
#include "Bsgsdk.h"
#include "Mihoyosdk.h"
#include "Scan.h"
#include "Login.h"
#include "QRScanner.h"


void scanMain(std::promise<std::string> url)
{
	Sleep(1500);
	Scan scan;
	scan.OpenVideo("../Honkai3StreamQRCode/cache/output.flv");
	int index = scan.GetStreamIndex(AVMEDIA_TYPE_VIDEO);
	int frameCount = 0;
	scan.FFmpegDecoder(index);
	scan.OpenDecoder(index);
	scan.buffer(scan.pFrameBGR);
	scan.swsctx(&scan.swsCtx);
	std::string qrCode;

	scan.opencvinit();

	int64_t latestTimestamp = av_gettime_relative();

	if (scan.avformatContext->streams[index]->start_time != AV_NOPTS_VALUE)
	{
		int64_t streamTimestamp = av_rescale_q(scan.avformatContext->streams[index]->start_time,
			scan.avformatContext->streams[index]->time_base, { 1, AV_TIME_BASE });
		if (streamTimestamp > latestTimestamp)
		{
			latestTimestamp = streamTimestamp;
		}
	}

	//av_seek_frame(scan.avformatContext, index, 10 * AV_TIME_BASE, 1);
	QRScanner s;
	while (/*scan.read(scan.avPacket) >= 0*/true)
	{
		scan.read(scan.avPacket);
		if (scan.avPacket->stream_index != index)
		{
			break;
		}
		avcodec_send_packet(scan.avCodecContext, scan.avPacket);
		while (scan.ReceiveFrame(scan.avframe) == 0)
		{
			av_seek_frame(scan.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
			// 转换像素格式
			sws_scale(scan.swsCtx, scan.avframe->data, scan.avframe->linesize, 0,
				scan.avCodecContext->height, scan.pFrameBGR->data, scan.pFrameBGR->linesize);

			if (frameCount % 15 == 0)
			{
				cv::Mat img(scan.avCodecContext->height, scan.avCodecContext->width, CV_8UC3, scan.pFrameBGR->data[0]);
				qrCode = s.Decode(img);
				imshow("Video", img);
				cv::waitKey(1);
			}
			frameCount++;
		}
		if (qrCode.find("biz_key=bh3_cn") != std::string::npos)
		{
			url.set_value(qrCode);
			break;
		}
		if (qrCode != "")
		{
			std::cout << "非崩坏3三二维码" << std::endl;
		}
		av_packet_unref(scan.avPacket);
	}
}

int main(int argc, char* argv[])
{
	v2api v2api;
	std::string playurl = v2api.Initialize();

	Login login("config_private.json");
	login.signedIn();
	login.setName();
	login.bh3Info();
	login.putConfigFile();

	Download down;
	std::thread th([&down, playurl]()
	{
		down.curlDownlod(playurl);
	});

	std::promise<std::string> QRcodeUrl;
	std::future<std::string> future_result = QRcodeUrl.get_future();
	std::thread th1(scanMain, std::move(QRcodeUrl));
	th1.join();
	std::string qrCode = future_result.get();
	std::cout << qrCode << std::endl;
	login.scanQRCode(qrCode);
	if (qrCode != "")
	{
		down.stopDownload();
		th.join();
	}
	std::cout << "Exit" << std::endl;
	return 0;
}