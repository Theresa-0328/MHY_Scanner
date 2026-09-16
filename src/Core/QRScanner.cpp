/**
 * @file QRScanner.cpp
 * @brief OpenCV WeChatQRCode 封装：模型加载、单帧/多码解码与调试日志。
 *
 * 模型路径相对 exe 目录解析，避免因工作目录不同导致加载失败。
 * qrLog 写入 exe 同目录 MHY_Scanner_debug.log，便于线上问题定位。
 */

#include "QRScanner.h"

#include <windows.h>
#include <filesystem>
#include <mutex>
#include <fstream>

// 模型路径改为相对 exe 所在目录解析，避免工作目录不同导致加载失败（表现为「无反应」）。
static std::filesystem::path getModelDir()
{
    WCHAR exePath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    return std::filesystem::path(exePath).parent_path() / "ScanModel";
}

/**
 * @brief 追加一行调试日志到 exe 目录下的 MHY_Scanner_debug.log。
 */
void qrLog(const std::string& msg)
{
    static std::mutex logMtx;
    std::lock_guard<std::mutex> lock(logMtx);
    WCHAR exePath[MAX_PATH]{};
    GetModuleFileNameW(nullptr, exePath, MAX_PATH);
    const auto logPath = std::filesystem::path(exePath).parent_path() / "MHY_Scanner_debug.log";
    std::ofstream f(logPath, std::ios::app);
    if (f.is_open())
    {
        f << msg << "\n";
    }
}

/**
 * @brief 构造解码器并加载 WeChatQRCode 四件套模型。
 */
QRScanner::QRScanner()
{
    const auto modelDir = getModelDir();
    try
    {
        detector = cv::makePtr<cv::wechat_qrcode::WeChatQRCode>(
            (modelDir / "detect.prototxt").string(),
            (modelDir / "detect.caffemodel").string(),
            (modelDir / "sr.prototxt").string(),
            (modelDir / "sr.caffemodel").string());
        detector->setScaleFactor(1.0);
        qrLog("WeChatQRCode model loaded OK");
    }
    catch (const std::exception& e)
    {
        qrLog(std::string("WeChatQRCode model load FAILED: ") + e.what());
    }
}

QRScanner::~QRScanner()
{
}

/**
 * @brief 解码单帧图像中的第一个二维码。
 * @param img 输入图像（BGR/BGRA 均可，内部会转灰度）。
 * @param qrCode 输出：解码结果，失败时保持空。
 */
void QRScanner::decodeSingle(const cv::Mat& img, std::string& qrCode)
{
    if (!detector)
    {
        return;
    }
#ifndef TESTSPEED
    auto startTime = std::chrono::high_resolution_clock::now();
#endif
    const std::vector<std::string>& strDecoded = detector->detectAndDecode(img);
    if (strDecoded.size() > 0)
    {
        qrCode = strDecoded[0];
    }
#ifndef TESTSPEED
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime).count();
    std::cout << static_cast<float>(duration) / 1000000 << " decode: " << qrCode << std::endl;
#endif
}

/**
 * @brief 解码图像中的多个二维码，结果写入 qrCode（保留最后一个）。
 */
void QRScanner::decodeMultiple(const cv::Mat& img, std::string& qrCode)
{
    if (!detector)
    {
        return;
    }
    const std::vector<std::string>& strDecoded = detector->detectAndDecode(img);
    for (int i = 0; i < strDecoded.size(); i++)
    {
        qrCode = strDecoded[i];
#ifdef _DEBUG
        std::cout << "decode:" << qrCode << std::endl;
#endif // DEBUG
    }
}
