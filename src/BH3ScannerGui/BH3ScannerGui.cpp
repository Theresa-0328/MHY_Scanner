#include "BH3ScannerGui.h"
#include <QMessageBox>
#include <QWindow>
#include <QRegularExpressionValidator>

BH3ScannerGui::BH3ScannerGui(QWidget* parent)
	: QMainWindow(parent)
	, t1(this)
	, t2(parent)
	, t3(this)
	, loginbili(parent)
{
	ui.setupUi(this);
	connect(ui.pBtLoginAccount, &QPushButton::clicked, this, &BH3ScannerGui::pBtLoginAccount);
	connect(ui.pBtstartScreen, &QPushButton::clicked, this, &BH3ScannerGui::pBtstartScreen);
	connect(ui.checkBoxAutoScreen, &QCheckBox::stateChanged, this, &BH3ScannerGui::checkBoxAutoScreen);
	connect(ui.checkBoxAutoExit, &QCheckBox::stateChanged, this, &BH3ScannerGui::checkBoxAutoExit);
	connect(ui.pBtStream, &QPushButton::clicked, this, &BH3ScannerGui::pBtStream);
	connect(&t1, &ThreadGetScreen::loginResults, this, &BH3ScannerGui::islogin);
	connect(&t3, &ThreadStreamProcess::loginSResults, this, &BH3ScannerGui::islogin);

	loginbili.openConfig();
	std::string readName;
	bool repeat = true;
	if (loginbili.loginBiliKey(readName) != 0)
	{
		repeat = false;
		QMessageBox::information(this,"提示","登录状态失效，\n请重新登录账号！",QMessageBox::Yes);
	}
	else
	{
		ui.pBtLoginAccount->setText("bilibili已登录");
		QString uname = QString::fromStdString(readName);
		ui.lineEditUname->setText(uname);
	}
	if (loginbili.getAutoStart()&&repeat)
	{
		ui.pBtstartScreen->clicked();
		ui.checkBoxAutoScreen->setChecked(true);
	}
	else if(loginbili.getAutoStart())
	{
		ui.checkBoxAutoScreen->setChecked(true);
	}
	if (loginbili.getAutoExit())
	{
		ui.checkBoxAutoExit->setChecked(true);
	}
	ui.lineEditLiveId->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$"), this));
	ui.lineEditLiveId->setClearButtonEnabled(true);
}

BH3ScannerGui::~BH3ScannerGui()
{
	
}

void BH3ScannerGui::pBtLoginAccount()
{
	ui.pBtLoginAccount->setEnabled(false);
	ui.pBtstartScreen->setEnabled(false);
	ui.pBtStream->setEnabled(false);
	if (t1.isRunning() || t2.isRunning() || t3.isRunning())
	{
		t1.stop();
		t2.stop();
		t3.stop();
		ui.pBtstartScreen->setText("开始监视屏幕");
		ui.pBtStream->setText("开始监视直播间");
	}
	loginwindow.exec();
	if (loginwindow.getIsReject())
	{
		ui.pBtLoginAccount->setEnabled(true);
		ui.pBtstartScreen->setEnabled(true);
		ui.pBtStream->setEnabled(true);
		return;
	}
	std::string account;
	std::string pwd;
	std::string message;
	loginwindow.getAccountPwd(account, pwd);
	int code = loginbili.loginBiliPwd(account, pwd, message);
	if (code == 0)
	{
		ui.pBtLoginAccount->setText("bilibili已登录");
		loginwindow.ClearInputBox();
		QString QName = QString::fromStdString(loginbili.getUName());
		ui.lineEditUname->setText(QName);
	}
	else
	{
		QString Qmessage = QString::fromLocal8Bit(message);
		std::wstring wlpstr = Qmessage.toStdWString();
		LPCWSTR lpcWStr = wlpstr.c_str();
		MessageBoxW(NULL, lpcWStr, L"bilibili登录失败", NULL);
		loginwindow.ClearInputBox();
	}
	ui.pBtLoginAccount->setEnabled(true);
	ui.pBtstartScreen->setEnabled(true);
	ui.pBtStream->setEnabled(true);
}

void BH3ScannerGui::pBtstartScreen()
{
	if (t1.isRunning())
	{
		t1.stop();
		ui.pBtstartScreen->setText("开始监视屏幕");
		return;
	}
	if (t3.isRunning())
	{
		t3.stop();
		t2.stop();
		ui.pBtStream->setText("开始监视直播间");
	}
	//选择和检查账号可用性
	std::string uName;
	if (loginbili.loginBiliKey(uName) != 0)
	{
		QMessageBox::information(this, "提示", "登录状态失效，\n请重新登录账号！", QMessageBox::Yes);
		return;
	}
	t1.InitScreen(loginbili.uid, loginbili.access_key, uName);
	ui.pBtstartScreen->setText("监视屏幕二维码中");
	t1.start();
}

void BH3ScannerGui::pBtStream()
{
	if (t1.isRunning())
	{
		t1.stop();
		ui.pBtstartScreen->setText("开始监视屏幕");
	}
	if (t3.isRunning())
	{
		t3.stop();
		t2.stop();
		ui.pBtStream->setText("开始监视直播间");
		return;
	}
	//选择和检查账号可用性
	std::string uName;
	if (loginbili.loginBiliKey(uName) != 0)
	{
		QMessageBox::information(this, "提示", "登录状态失效，\n请重新登录账号！", QMessageBox::Yes);
		return;
	}
	//检查直播间状态
	QString liveRoomId = ui.lineEditLiveId->text();
	std::string n = liveRoomId.toStdString();
	v2api v;
	int id = v.GetRealRoomID(n);
	int readId = liveIdError(id);
	if (readId == 0)
		return;
	std::string streamAddress = v.GetAddress(readId);
	t2.downloadInit(streamAddress);
	ui.pBtStream->setText("监视直播二维码中");
	t2.start();
	t3.biliInitStream(loginbili.uid, loginbili.access_key, uName);
	t3.start();
}

void BH3ScannerGui::closeEvent(QCloseEvent* event)
{
	t1.stop();
	t3.stop();
	t2.stop();
}

void BH3ScannerGui::showEvent(QShowEvent* event)
{

}

void BH3ScannerGui::islogin(const bool& b)
{
	t1.stop();
	t2.stop();
	t3.stop();
	if (b)
	{
		if (loginbili.getAutoExit())
		{
			exit(0);
		}
		ui.pBtstartScreen->setText("开始监视屏幕");
		ui.pBtStream->setText("开始监视直播间");
		QMessageBox::information(this, "提示", "扫码成功！", QMessageBox::Yes);
	}
	else
	{
		ui.pBtstartScreen->setText("开始监视屏幕");
		ui.pBtStream->setText("开始监视直播间");
		QMessageBox::information(this, "提示", "扫码失败", QMessageBox::Yes);
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

int BH3ScannerGui::liveIdError(int code)
{
	switch (code)
	{
	case -1:
	{
		QMessageBox::information(this,"提示", "直播间不存在!", QMessageBox::Yes);

	}
		return 0;
	case -2:
	{
		QMessageBox::information(this, "提示", "直播间未开播！", QMessageBox::Yes);

	}
		return 0;
	case -3:
	{
		QMessageBox::information(this,"提示", "未知错误!", QMessageBox::Yes);
	}
		return 0;
	default:
		return code;
	}
}