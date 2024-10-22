#pragma once

#include <array>
#include <iostream>

#include <QTabwidget>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QBoxLayout>

class WindowLogin : public QWidget
{
    Q_OBJECT
public:
    WindowLogin(QWidget* parent = nullptr);
    ~WindowLogin();
    Q_DISABLE_COPY(WindowLogin)
signals:
    void signals_test1();
private slots:

private:
    std::array<QWidget*, 4> tabs{};
    static constexpr std::array<const char*, 4> tabsName{ "短信登录", "扫码登录", "Cookie登录", "Bilibili登录" };
    QTabWidget* tabWidget{};
    QHBoxLayout* MainHLayout{};

    void InitTabs0();
    QHBoxLayout* Tab0MainHLayout;
    QLineEdit* lineEditCookie{};
    QPushButton* pBtofficialLogin{};
    QPushButton* pBtofficialreject{};
    QHBoxLayout* Tab0ButtonHLayout;
    QVBoxLayout* Tab0VLayout;

    void InitTabs1();
    QHBoxLayout* Tab1MainHLayout;

    void InitTabs2();
    QHBoxLayout* Tab2MainHLayout;

    void InitTabs3();
    QHBoxLayout* Tab3MainHLayout;

    void Initconnect();
};