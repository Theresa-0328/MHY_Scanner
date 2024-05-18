#pragma once

#include <thread>
#include <mutex>
#include <string>
#include <string_view>
#include <iostream>

#include <QDialog>
#include <QLabel>
#include <QImage>
#include <QPainter>
#include <QThreadPool>
#include <QtWidgets/QHBoxLayout>
#include <opencv2/opencv.hpp>

#include "ui_LoginWindow.h"
#include "UtilMat.hpp"

enum class LoginType
{
    OfficialCookie = 0,
    OfficialQRCode = 1,
    BH3Bili = 2
};

class LoginWindow : public QDialog
{
    Q_OBJECT
public:
    LoginWindow(QDialog* Dialog = nullptr);
    ~LoginWindow();
    void getAccountPwd(std::string& Pwd, std::string& Account);
    std::string cookie;
    int type = 0;
    bool getIsReject();
public slots:
    void showPassword(int state);
    void accept() override;
    void reject() override;
    void officialLogin();
    void officialreject();
    void controlQRCodeImage(int index);
    void closeEvent(QCloseEvent* event) override;
signals:
    void sendQrcodeButtonVisible();

private:
    Ui::Dialog ui;
    void ClearInputBox();
    void qrcodeThreadFun(int index);
    void hideQrcodeButtonFun();
    void showQrcodeButtonFun();
    std::string Pwd;
    std::string Account;
    bool isReject = false;
    QWidget* QRCodewidget;
    QImage QRCodeImage;
    QLabel* QRCodelabel;
    bool showQRcodeImage;
    QPushButton* UpdateQrcodeButton;
    QThreadPool threadPool;
    cv::Mat QrcodeMat;
};
