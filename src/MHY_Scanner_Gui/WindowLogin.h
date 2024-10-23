#pragma once

#include <array>
#include <iostream>

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

private slots:

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    WindowGeeTest m_WindowGeeTest{ this };
    std::string GeetestSessionId{};
    QThreadPool thpool;
    std::array<QWidget*, 4> tabs{};
    static constexpr std::array<const char*, 4> tabsName{ "短信登录", "扫码登录", "Cookie登录", "Bilibili登录" };
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
    QHBoxLayout* Tab1MainHLayout{};

    void InitTabs2();
    QVBoxLayout* Tab2MainVLayout{};
    QLineEdit* lineEditCookie{};
    QPushButton* pBtofficialLogin{};
    QPushButton* pBtofficialreject{};
    QHBoxLayout* Tab2ButtonHLayout{};

    void InitTabs3();
    QHBoxLayout* Tab3MainHLayout{};

    void Initconnect();
};