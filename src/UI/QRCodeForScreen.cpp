/**
 * @file QRCodeForScreen.cpp
 * @brief 屏幕监视扫码：DXGI 采集桌面帧，WeChatQRCode 解码后走 passport 登录链路。
 *
 * DXGI 在画面无变化时会超时，静止二维码需要复用上一帧继续解码。
 * 采集失败必须写日志并跳过，避免把全 0 缓冲区当作画面。
 */

#include "QRCodeForScreen.h"

#include <chrono>
#include <thread>

#include <QFuture>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>

#include "QRScanner.h"
#include "ScreenScan.h"
#include "ScreenShotDXGI.hpp"

#define DELAYED 200

QRCodeForScreen::QRCodeForScreen(QObject* parent) :
    QThread(parent),
    m_stop(false)
{
    m_config = &ConfigDate::getInstance();
}

QRCodeForScreen::~QRCodeForScreen()
{
    if (!this->isInterruptionRequested())
    {
        m_stop.store(false);
    }
    this->requestInterruption();
    this->wait();
}

void QRCodeForScreen::setLoginInfo(const std::string& uid, const std::string& token)
{
    this->uid = uid;
    this->stoken = token;
}

void QRCodeForScreen::setLoginInfo(const std::string& uid, const std::string& token, const std::string& name)
{
    this->uid = uid;
    this->stoken = token;
    this->m_name = name;
}

void QRCodeForScreen::setMid(const std::string& mid)
{
    this->mid = mid;
}

void QRCodeForScreen::LoginOfficial()
{
    QThreadPool threadPool;
    threadPool.setMaxThreadCount(threadNumber);
    std::mutex mtx;
    ScreenShotDXGI screenshotdxgi;
    int w{ 0 };
    int h{ 0 };
    if (!screenshotdxgi.InitDevice() || !screenshotdxgi.InitDupl(0, w, h))
    {
        ret = ScanRet::STREAMERROR;
        Q_EMIT loginResults(ret);
        return;
    }
    long mBufferSize = w * h * 4;
    uint8_t* mBuffer = new UCHAR[mBufferSize]();
    qrLog("Screen monitor: w=" + std::to_string(w) + " h=" + std::to_string(h));
    int frameCount = 0;
    cv::Mat lastGoodFrame;
    while (m_stop.load())
    {
        const int frameResult = screenshotdxgi.getFrame(100);
        if (frameResult == 1)
        {
            ret = ScanRet::STREAMERROR;
            Q_EMIT loginResults(ret);
            break;
        }

        cv::Mat img;
        if (frameResult == 0)
        {
            if (!screenshotdxgi.copyFrameToBuffer(&mBuffer, mBufferSize))
            {
                qrLog("copyFrameToBuffer failed");
                screenshotdxgi.doneWithFrame();
                std::this_thread::sleep_for(std::chrono::milliseconds(DELAYED));
                continue;
            }
            cv::resize(cv::Mat(h, w, CV_8UC4, mBuffer), img, { 1280, 720 });
            lastGoodFrame = img;
            ++frameCount;
            if (frameCount <= 5)
            {
                qrLog("frame captured #" + std::to_string(frameCount));
            }
            if (frameCount == 1)
            {
                cv::imwrite("MHY_Scanner_frame.png", img);
                qrLog("saved first frame to MHY_Scanner_frame.png");
            }
#ifndef SHOW
            cv::imshow("Video_Stream", img);
            cv::waitKey(1);
#endif
            screenshotdxgi.doneWithFrame();
        }
        else
        {
            // DXGI 在画面无变化时超时；静止二维码只会在「刚出现」那一帧变化，
            // 这里复用上一帧继续解码，避免「码一直摆着却无反应」。
            if (lastGoodFrame.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(DELAYED));
                continue;
            }
            img = lastGoodFrame.clone();
        }

        threadPool.tryStart([&, img = std::move(img)]() {
            thread_local QRScanner qrScanners;
            std::string str;
            qrScanners.decodeSingle(img, str);
            if (!str.empty())
            {
                qrLog("decoded: " + str.substr(0, 160));
            }
            if (str.size() < 85)
            {
                return;
            }
            std::string_view view(str.c_str() + 79, 3);
            if (!setGameType.contains(view))
            {
                return;
            }
            setGameType[view]();
            const std::string_view ticket(str.data() + str.size() - 24, 24);
            if (lastTicket == ticket)
            {
                return;
            }
            if (mtx.try_lock())
            {
                if (!m_stop.load())
                {
                    mtx.unlock();
                    return;
                }
                passportQRUrl = PandaScanQRCode(scanUrl.data(), ticket, gameType);
                if (!passportQRUrl.empty() && PassportQRLogin(passportQRUrl, stoken, mid, false))
                {
                    lastTicket = ticket;
                    nlohmann::json config = nlohmann::json::parse(m_config->getConfig());
                    if (config["auto_login"])
                    {
                        continueLastLogin();
                    }
                    else
                    {
                        emit loginConfirm(gameType, true);
                    }
                }
                else
                {
                    emit loginResults(ScanRet::FAILURE_1);
                }
                stop();
                mtx.unlock();
            }
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAYED));
    }
    delete[] mBuffer;
}

void QRCodeForScreen::LoginBH3BiliBili()
{
    QThreadPool threadPool;
    threadPool.setMaxThreadCount(threadNumber);
    std::mutex mtx;
    ScreenShotDXGI screenshotdxgi;
    int w{ 0 };
    int h{ 0 };
    if (!screenshotdxgi.InitDevice() || !screenshotdxgi.InitDupl(0, w, h))
    {
        ret = ScanRet::STREAMERROR;
        Q_EMIT loginResults(ret);
        return;
    }
    long mBufferSize = w * h * 4;
    uint8_t* mBuffer = new UCHAR[mBufferSize]();
    qrLog("Screen monitor: w=" + std::to_string(w) + " h=" + std::to_string(h));
    int frameCount = 0;
    cv::Mat lastGoodFrame;
    while (m_stop.load())
    {
        const int frameResult = screenshotdxgi.getFrame(100);
        if (frameResult == 1)
        {
            ret = ScanRet::STREAMERROR;
            Q_EMIT loginResults(ret);
            break;
        }

        cv::Mat img;
        if (frameResult == 0)
        {
            if (!screenshotdxgi.copyFrameToBuffer(&mBuffer, mBufferSize))
            {
                qrLog("copyFrameToBuffer failed");
                screenshotdxgi.doneWithFrame();
                std::this_thread::sleep_for(std::chrono::milliseconds(DELAYED));
                continue;
            }
            cv::resize(cv::Mat(h, w, CV_8UC4, mBuffer), img, { 1280, 720 });
            lastGoodFrame = img;
            ++frameCount;
            if (frameCount <= 5)
            {
                qrLog("frame captured #" + std::to_string(frameCount));
            }
            if (frameCount == 1)
            {
                cv::imwrite("MHY_Scanner_frame.png", img);
                qrLog("saved first frame to MHY_Scanner_frame.png");
            }
#ifndef SHOW
            cv::imshow("Video_Stream", img);
            cv::waitKey(1);
#endif
            screenshotdxgi.doneWithFrame();
        }
        else
        {
            if (lastGoodFrame.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(DELAYED));
                continue;
            }
            img = lastGoodFrame.clone();
        }

        threadPool.tryStart([&, img = std::move(img)]() {
            thread_local QRScanner qrScanners;
            std::string str;
            qrScanners.decodeSingle(img, str);
            if (!str.empty())
            {
                qrLog("decoded: " + str.substr(0, 160));
            }
            if (str.size() < 85)
            {
                return;
            }
            if (std::string_view view(str.c_str() + 79, 3); view != "8F3")
            {
                return;
            }
            const std::string& ticket = str.substr(str.length() - 24);
            if (lastTicket == ticket)
            {
                return;
            }
            if (mtx.try_lock())
            {
                if (!m_stop.load())
                {
                    mtx.unlock();
                    return;
                }
                if (ret = scanCheck(ticket); ret == ScanRet::SUCCESS)
                {
                    lastTicket = ticket;
                    nlohmann::json config = nlohmann::json::parse(m_config->getConfig());
                    if (config["auto_login"])
                    {
                        continueLastLogin();
                    }
                    else
                    {
                        emit loginConfirm(GameType::Honkai3_BiliBili, true);
                    }
                }
                else
                {
                    emit loginResults(ScanRet::FAILURE_1);
                }
                stop();
                mtx.unlock();
            }
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(DELAYED));
    }
    delete[] mBuffer;
}

void QRCodeForScreen::continueLastLogin()
{
    switch (servertype)
    {
        using enum ServerType;
    case Official:
    {
        bool b = PassportQRLogin(passportQRUrl, stoken, mid, true);
        if (b)
        {
            Q_EMIT loginResults(ScanRet::SUCCESS);
        }
        else
        {
            Q_EMIT loginResults(ScanRet::FAILURE_2);
        }
    }
    break;
    case BH3_BiliBili:
    {
        ret = scanConfirm(lastTicket, uid, stoken, m_name);
        Q_EMIT loginResults(ret);
    }
    break;
    default:
        break;
    }
}

void QRCodeForScreen::run()
{
    ret = ScanRet::UNKNOW;
    m_stop.store(true);
#ifndef SHOW
    cv::namedWindow("Video_Stream", cv::WINDOW_AUTOSIZE);
#endif
    switch (servertype)
    {
    case ServerType::Official:
        LoginOfficial();
        break;
    case ServerType::BH3_BiliBili:
        LoginBH3BiliBili();
        break;
    default:
        break;
    }
#ifndef SHOW
    cv::destroyWindow("Video_Stream");
#endif
}

void QRCodeForScreen::stop()
{
    m_stop.store(false);
}

void QRCodeForScreen::setServerType(const ServerType servertype)
{
    this->servertype = servertype;
}
