#pragma once

#include <QtWidgets/QMainWindow>
#include <Json.h>

#include "ui_ScannerGui.h"
#include "QRCodeForScreen.h"
#include "QRCodeForStream.h"
#include "LiveStreamLink.h"
#include "Common.h"

class OnlineUpdate :public QThread
{
public:
	OnlineUpdate() = default;
	void run();
	~OnlineUpdate();
};

class ScannerGui
	: public QMainWindow
{
	Q_OBJECT

public:
	ScannerGui(QWidget* parent = nullptr);
	~ScannerGui();
	void insertTableItems(QString uid, QString userName, QString type, QString notes);
	virtual void closeEvent(QCloseEvent* event);
	virtual void showEvent(QShowEvent* event);
public slots:
	void pBtLoginAccount();
	void pBtstartScreen();
	void islogin(const ScanRet::Type ret);
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
	QRCodeForScreen t1;
	ThreadStreamProcess t2;
	json::Json userinfo;
	int liveIdError(const LiveStreamStatus::Status data);
	std::string readConfigFile(const std::string& filePath);
	void createDefaultConfigFile(const std::string& filePath, std::string defaultConfig);
	void updateConfig();
	void updateUserinfo(const std::string& str);
	void loadUserinfo();
	std::string loadConfig();
	int getSelectedRowIndex();
	bool checkDuplicates(const std::string uid);
	bool getStreamLink(const std::string& roomid, std::string& url, std::map<std::string, std::string>& heards);
	OnlineUpdate o;
};