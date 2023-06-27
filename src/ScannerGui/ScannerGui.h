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
    void insertTableItems(QString uid, QString userName, QString type, QString notes);
    virtual void closeEvent(QCloseEvent* event);
    virtual void showEvent(QShowEvent* event);
public slots:
    void pBtLoginAccount();
    void pBtstartScreen();
    void islogin(const bool& b);
    void checkBoxAutoScreen(int state);
    void checkBoxAutoExit(int state);
    void pBtStream();
    void getInfo(int x, int y);
    void pBtSwitch();
    void pBtDeleteAccount();
private:
    void failure();
    int countA = -1;
    Ui::ScannerGuiClass ui;
    json::Json configJson;
    ThreadGetScreen t1;
    ThreadStreamProcess t2;
    LoginWindow loginwindow;
    json::Json userinfo;
    int liveIdError(int code);
    std::string readConfigFile(const std::string& filePath);
    void createDefaultConfigFile(const std::string& filePath, std::string defaultConfig);
    void updateConfig();
    void updateUserinfo(const std::string& str);
    void loadUserinfo();
    std::string loadConfig();
    int getSelectedRowIndex();
};