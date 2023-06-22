#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_ScannerGui.h"
#include "LoginBili.h"
#include "ThreadGetScreen.h"
#include "LoginWindow.h"
#include "ThreadStreamProcess.h"
#include "BiliLiveApi.h"

class ScannerGui 
    : public QMainWindow
{
    Q_OBJECT

public:
    ScannerGui(QWidget *parent = nullptr);
    ~ScannerGui();
    virtual void closeEvent(QCloseEvent* event);
    virtual void showEvent(QShowEvent* event);
public slots:
    void pBtLoginAccount();
    void pBtstartScreen();
    void islogin(const bool& b);
    void checkBoxAutoScreen(int state);
    void checkBoxAutoExit(int state);
    void pBtStream();
    void failure();
private:
    Ui::ScannerGuiClass ui;
    ThreadGetScreen t1;
    ThreadStreamProcess t3;
    LoginBili loginbili;
    LoginWindow loginwindow;
    int liveIdError(int code);
};