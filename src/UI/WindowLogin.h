#pragma once

#include <array>
#include <iostream>
#include <atomic>

#include <opencv2/opencv.hpp>
#include <QTabwidget>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>
#include <QLabel>
#include <QThread>
#include <QThreadPool>
#include <QMessagebox>
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QCheckBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTimer>
#include <QImage>
#include <QMutex>

#include "WindowGeeTest.h"

class WindowLogin : public QWidget
{
    Q_OBJECT
public:
    WindowLogin(QWidget* parent = nullptr);
    ~WindowLogin();
    Q_DISABLE_COPY(WindowLogin)

signals:
    void showMessagebox(const QString& Message);
    void showWindowGeeTest(const bool show);
    void AddUserInfo(const std::string& name, const std::string token, const std::string uid, const std::string mid, const std::string type);
    void QrcodeLoginResult(bool result);
    void StartQRCodeTimer();

private slots:

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    WindowGeeTest m_WindowGeeTest{ this };
    QThreadPool thpool{};
    std::array<QWidget*, 4> tabs{};
    static constexpr std::array<const char*, 4> tabsName{ "短信登录", "扫码登录", "Cookie登录", "Bilibili崩坏3登录" };
    QTabWidget* tabWidget{};
    QHBoxLayout* MainHLayout{};

    void InitTabs0();
    QVBoxLayout* Tab0MainVLayout{};
    QLabel* AreaCodeLabel{};
    QLineEdit* phoneNumberLineEdit{};
    QPushButton* pbtSend{};
    QHBoxLayout* Tab0HLayout0{};
    QLineEdit* verifyCodeLineEdit{};
    QHBoxLayout* Tab0HLayout1{};
    QPushButton* Tab0pbtConfirm{};
    QPushButton* Tab0pbtCancel{};
    QHBoxLayout* Tab0HLayout2{};

    void InitTabs1();
    QVBoxLayout* Tab1MainVLayout{};
    QPushButton* UpdateQrcodeButton{};
    QImage QRCodeQImage{ 305, 305, QImage::Format_Indexed8 };
    cv::Mat QrcodeMat{};
    inline static QThreadPool QrcodePool{};
    QLabel* QRCodelabel{};
    QLabel* Prompt{};
    QTimer* QrcodeTimer;
    std::string ticket;
    std::atomic_bool AllowDrawQRCode{};

    void InitTabs2();
    QVBoxLayout* Tab2MainVLayout{};
    QLineEdit* lineEditCookie{};
    QPushButton* pBtofficialLogin{};
    QPushButton* pBtofficialreject{};
    QHBoxLayout* Tab2ButtonHLayout{};

    void InitTabs3();
    QVBoxLayout* Tab3MainVLayout{};
    QVBoxLayout* Tab3VLayout0{};
    QLineEdit* lineEditAccount{};
    QLineEdit* lineEditPwd{};
    QVBoxLayout* Tab3VLayout1{};
    QCheckBox* checkBoxShowPw{};
    QHBoxLayout* Tab3HLayout2{};
    QPushButton* Tab3pBtConfirm{};
    QPushButton* Tab3pBtCancel{};

    void Initconnect();

    struct
    {
        std::string GeetestSessionId{};
        std::string action_type{};
        std::string Aigis{};
        std::string phoneNumber{};
        std::string gt_user_id{};
        enum
        {
            Official,
            BiLi
        } GeeTestType{};
    } GeeTestInfo;

    void ResultByLoginBH3BiLiBiLi(const auto& result);
    void ResultByLoginOfficial(const auto& result);
    void StartQRCodeLogin();
    void CheckQRCodeLoginState();
};