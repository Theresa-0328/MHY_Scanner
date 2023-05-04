#include "BH3ScannerGui.h"
#include <QMessageBox>
#include <QWindow>

BH3ScannerGui::BH3ScannerGui(QWidget* parent)
	: QMainWindow(parent)
	, t1(this)
{
	ui.setupUi(this);
	bool b1 = connect(ui.pBtLoginAccount, &QPushButton::clicked, this, &BH3ScannerGui::pBtLoginAccount);
	bool b2 = connect(ui.pBtstartScreen, &QPushButton::clicked, this, &BH3ScannerGui::pBtstartScreen);
	bool b3 = connect(&t1, &ThreadGetScreen::loginResults, this, &BH3ScannerGui::islogin);
	bool b4 = connect(ui.checkBoxAutoScreen, SIGNAL(stateChanged(int)),this, SLOT(checkBoxAutoScreen(int)));
	bool b5 = connect(ui.checkBoxAutoExit, SIGNAL(stateChanged(int)), this, SLOT(checkBoxAutoExit(int)));
	bool b6 = connect(ui.pBtStream, &QPushButton::clicked, this, &BH3ScannerGui::pBtStream);
	loginbili.openConfig();
	std::string readName;
	if (loginbili.loginBiliKey(readName) != 0)
	{
		ui.pBtstartScreen->setText("开始监视屏幕");
		QMessageBox msgBox(QMessageBox::Information,
			"提示",
			"登录状态失效，\n请重新登录账号！",
			QMessageBox::Yes,
			this);
		msgBox.exec();
		return;
	}
	else 
	{
		ui.pBtLoginAccount->setText("bilibili已登录");
	}
	if (loginbili.getAutoStart())
	{
		ui.pBtstartScreen->clicked();
		ui.checkBoxAutoScreen->setChecked(true);
	}
	else
	{
		ui.checkBoxAutoScreen->setChecked(false);
	}
	if (loginbili.getAutoExit())
	{
		ui.checkBoxAutoExit->setChecked(true);
	}
}

BH3ScannerGui::~BH3ScannerGui()
{
}

void BH3ScannerGui::pBtLoginAccount()
{
	l.exec();
	if (l.getIsReject())
	{
		return;
	}
	std::string a1;
	std::string a2;
	std::string a3;
	l.getAccountPwd(a1, a2);
	int c = loginbili.loginBiliPwd(a1, a2, a3);
	if (c == 0)
	{
		ui.pBtLoginAccount->setText("bilibili已登录");
		l.ClearInputBox();
	}
	else
	{
		QString myStr = QString::fromLocal8Bit(a3);
		std::wstring wlpstr = myStr.toStdWString();
		LPCWSTR lpcWStr = wlpstr.c_str();
		MessageBoxW(NULL, lpcWStr, L"bilibili登录失败", NULL);
		l.ClearInputBox();
	}
}

void BH3ScannerGui::pBtstartScreen()
{
	if (t1.isExit == false)
	{
		t1.isExit = true;
		ui.pBtstartScreen->setText("开始");
		return;
	}
	//选择账号
	std::string readName;
	if (loginbili.loginBiliKey(readName) != 0)
	{
		ui.pBtstartScreen->setText("开始监视屏幕");
		QMessageBox msgBox(QMessageBox::Information,
			"提示",
			"登录状态失效，\n请重新登录账号！",
			QMessageBox::Yes,
			this);
		msgBox.exec();
		return;
	}
	else
	{
		ui.pBtLoginAccount->setText("bilibili已登录");
	}
	t1.biliInit(loginbili.uid, loginbili.access_key, readName);
	ui.pBtstartScreen->setText("监视屏幕二维码中");
	t1.start();
}

void BH3ScannerGui::closeEvent(QCloseEvent* event)
{

}

void BH3ScannerGui::showEvent(QShowEvent* event)
{

}

void BH3ScannerGui::islogin(const bool& b)
{
	if (b)
	{
		if (loginbili.getAutoExit())
		{
			exit(0);
		}
		QMessageBox msgBox(QMessageBox::Information,
			"提示",
			"扫码成功！",
			QMessageBox::Yes,
			this);
		ui.pBtstartScreen->setText("开始获取屏幕二维码");
		msgBox.exec();
	}
	else
	{
		ui.pBtstartScreen->setText("开始获取屏幕二维码");
	}
}

void BH3ScannerGui::checkBoxAutoScreen(int state)
{
	if (state == Qt::Checked)
	{
		loginbili.setAutoStart(true);
	}
	else if (state == Qt::Unchecked)
	{
		loginbili.setAutoStart(false);
	}
}

void BH3ScannerGui::checkBoxAutoExit(int state)
{
	if (state == Qt::Checked)
	{
		loginbili.setAutoExit(true);
	}
	else if (state == Qt::Unchecked)
	{
		loginbili.setAutoExit(false);
	}
}

void BH3ScannerGui::pBtStream()
{

}
