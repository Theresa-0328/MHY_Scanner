#pragma once

#include <thread>

#include <QDialog>
#include <QLabel>
#include <QImage>
#include <QPainter>

#include "ui_LoginWindow.h"

class LoginWindow : public QDialog
{
    Q_OBJECT
public:
    LoginWindow(QDialog* Dialog = nullptr);
    void getAccountPwd(std::string& Pwd, std::string& Account);
    std::string cookie;
    int type = 0;
    bool getIsReject();
public slots:
    void showPassword(int state);
    virtual void accept();
    virtual void reject();
    void officialLogin();
    void officialreject();
    void controlQRCodeImage(int index);

private:
    Ui::Dialog ui;
    void ClearInputBox();
    std::string Pwd;
    std::string Account;
    bool isReject = false;
    QWidget* widget;
    QImage QRCodeImage;
    QLabel* label;
};
