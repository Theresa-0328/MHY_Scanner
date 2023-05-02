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
#include "VideoProcessor.h"
#include "Login.h"
#include "QRScanner.h"

void scanMain(std::promise<std::string> url)
{
	Sleep(1500);
	VideoProcessor scan;
	scan.OpenVideo("./cache/output.flv");
	int index = scan.GetStreamIndex(AVMEDIA_TYPE_VIDEO);
	int frameCount = 0;
	scan.FFmpegDecoder(index);
	scan.OpenDecoder(index);
	scan.buffer(scan.pFrameBGR);
	scan.swsctx(&scan.swsCtx);

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

	av_seek_frame(scan.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
	//namedWindow("Video", cv::WINDOW_NORMAL);
	//cv::namedWindow("Video", cv::WINDOW_NORMAL);
	//cv::resizeWindow("Video", 1280, 720);
	QRScanner s;
	std::string qrCode;
	int fff = 0;
	while (true)
	{
		//av_seek_frame(scan.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
		//av_seek_frame(scan.avformatContext, -1, 60, AVSEEK_FLAG_FRAME);
		int op1 = scan.read(scan.avPacket);
		fff++;
		if (scan.avPacket->stream_index != index)
		{
			continue;
		}
		int op2 = avcodec_send_packet(scan.avCodecContext, scan.avPacket);
		if(op2!=0)
		{
			cv::waitKey(1000);
		}
		while (true)
		{
			if (scan.ReceiveFrame(scan.avframe) != 0)
			{
				cv::waitKey(1000);
				break;
			}

			// 转换像素格式
			sws_scale(scan.swsCtx, scan.avframe->data, scan.avframe->linesize, 0,
				scan.avCodecContext->height, scan.pFrameBGR->data, scan.pFrameBGR->linesize);
			cv::Mat img(scan.avCodecContext->height, scan.avCodecContext->width, CV_8UC3, scan.pFrameBGR->data[0]);
			if (fff > 90)
			{
				s.Decode(img, qrCode);
				fff = 0;
				std::cout << "已命中" << std::endl;
				//av_seek_frame(scan.avformatContext, -1, latestTimestamp, AVSEEK_FLAG_BACKWARD);
			}
			imshow("Video", img);
			cv::waitKey(1);
			break;
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
	std::string streamAddress = v2api.GetAddress();

	//Login login("config_private.json");
	//login.signedIn();
	//login.setName();
	//login.bh3Info();
	//login.putConfigFile();

	Download down;
	std::thread th([&down, streamAddress]()
	{
		down.curlDownlod(streamAddress);
	});

	std::promise<std::string> QRcodeUrl;
	std::future<std::string> future_result = QRcodeUrl.get_future();
	std::thread th1(scanMain, std::move(QRcodeUrl));
	th1.join();
	std::string qrCode = future_result.get();
	std::cout << qrCode << std::endl;
	//login.scanQRCode(qrCode);
	if (qrCode != "")
	{
		down.stopDownload();
		th.join();
	}
	std::cout << "Exit" << std::endl;
	return 0;
}