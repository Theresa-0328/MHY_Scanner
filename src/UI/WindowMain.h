#pragma once

#include <vector>

#include <QRunnable>
#include <QMainWindow>

#include "ApiDefs.hpp"
#include "ConfigDate.h"
#include "LiveStreamLink.h"
#include "QRCodeForScreen.h"
#include "QRCodeForStream.h"
#include "ui_WindowMain.h"
#include "WindowLogin.h"
#include "WindowAbout.h"

class OnlineUpdate :
    public QThread
{
public:
    OnlineUpdate() = default;
    void run();
    ~OnlineUpdate();
};

class configInitLoad :
    public QThread
{
    Q_OBJECT
public:
    configInitLoad() = default;
    void run();
    ~configInitLoad();
signals:
    void userinfoTrue();

private:
    ConfigDate* m_config = &ConfigDate::getInstance();
};

class WindowMain :
    public QMainWindow
{
    Q_OBJECT

public:
    WindowMain(QWidget* parent = nullptr);
    ~WindowMain();
    void insertTableItems(QString uid, QString userName, QString type, QString notes);
    void closeEvent(QCloseEvent* event) override;
    void showEvent(QShowEvent* event) override;
public Q_SLOTS:
    void AddAccount();
    void SetDefaultAccount();
    void DeleteAccount();

    void pBtstartScreen(bool clicked);
    void pBtStream(bool clicked);
    void pBtStop();

    void checkBoxAutoScreen(bool clicked);
    void checkBoxAutoExit(bool clicked);
    void checkBoxAutoLogin(bool clicked);

    void islogin(const ScanRet ret);
    void loginConfirmTip(const GameType gameType, bool b);
    void getInfo(int x, int y);
    void configInitUpdate();
    void updateNote(QTableWidgetItem* item);
Q_SIGNALS:
    void StopScanner();
    void AccountError();
    void LiveStreamLinkError(LiveStreamStatus status);
    void AccountNotSelected();
    void StartScanScreen();
    void StartScanLive();

private:
    void failure();
    int countA = -1;
    Ui::WindowMain ui;
    ConfigDate* m_config = &ConfigDate::getInstance();
    nlohmann::json userinfo;
    QRCodeForScreen t1;
    QRCodeForStream t2;
    void liveIdError(const LiveStreamStatus status);
    int getSelectedRowIndex();
    bool checkDuplicates(const std::string uid);
    bool GetStreamLink(const std::string& roomid, std::string& url, std::map<std::string, std::string>& heards);
    OnlineUpdate o;
    configInitLoad configinitload;
    void SetWindowToFront() const;
    WindowLogin* windowLogin{};
    void copyEntireRow(int row);
    void onTableRightClicked(const QPoint& pos);
};