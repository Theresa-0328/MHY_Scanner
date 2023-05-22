#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BH3ScannerGui.h"
#include "LoginBili.h"
#include "ThreadGetScreen.h"
#include "ThreadDownload.h"
#include "LoginWindow.h"
#include "ThreadStreamProcess.h"
#include "BiliLiveApi.h"

class BH3ScannerGui 
    : public QMainWindow
{
    Q_OBJECT

public:
    BH3ScannerGui(QWidget *parent = nullptr);
    ~BH3ScannerGui();
    virtual void closeEvent(QCloseEvent* event);
    virtual void showEvent(QShowEvent* event);
public slots:
    void pBtLoginAccount();
    void pBtstartScreen();
    void islogin(const bool& b);
    void checkBoxAutoScreen(int state);
    void checkBoxAutoExit(int state);
    void pBtStream();
private:
    Ui::BH3ScannerGuiClass ui;
    ThreadGetScreen t1;
    ThreadDownload t2;
    ThreadStreamProcess t3;
    LoginBili loginbili;
    LoginWindow l;
    int liveIdError(int code);
    HANDLE processMutex;
};