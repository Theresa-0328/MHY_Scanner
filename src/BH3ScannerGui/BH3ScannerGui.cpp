#include "BH3ScannerGui.h"
#include <QMessageBox>
#include <QWindow>
#include <QRegularExpressionValidator>

BH3ScannerGui::BH3ScannerGui(QWidget* parent)
	: QMainWindow(parent)
	, t1(this)
	, t2(this)
	, t3(this)
{
	ui.setupUi(this);
	bool b1 = connect(ui.pBtLoginAccount, &QPushButton::clicked, this, &BH3ScannerGui::pBtLoginAccount);
	bool b2 = connect(ui.pBtstartScreen, &QPushButton::clicked, this, &BH3ScannerGui::pBtstartScreen);
	bool b3 = connect(&t1, &ThreadGetScreen::loginResults, this, &BH3ScannerGui::islogin);
	bool b4 = connect(ui.checkBoxAutoScreen, &QCheckBox::stateChanged, this, &BH3ScannerGui::checkBoxAutoScreen);
	bool b5 = connect(ui.checkBoxAutoExit, &QCheckBox::stateChanged, this, &BH3ScannerGui::checkBoxAutoExit);
	bool b6 = connect(ui.pBtStream, &QPushButton::clicked, this, &BH3ScannerGui::pBtStream);
	bool b7 = connect(&t3, &ThreadStreamProcess::loginSResults, this, &BH3ScannerGui::islogin);

	loginbili.openConfig();
	std::string readName;
	if (loginbili.loginBiliKey(readName) != 0)
	{
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
		QString uname = QString::fromStdString(readName);
		ui.lineEditUname->setText(uname);
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
	ui.lineEditLiveId->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$"), this));
	ui.lineEditLiveId->setClearButtonEnabled(true);
}

BH3ScannerGui::~BH3ScannerGui()
{
}

void BH3ScannerGui::pBtLoginAccount()
{
	if (t1.isRunning() || t2.isRunning() || t3.isRunning())
	{
		t1.isExit = true;
		ui.pBtstartScreen->setText("开始监视屏幕");
		t3.stop();
		t2.stopDownload();
		ui.pBtStream->setText("开始监视直播间");
	}
	l.exec();
	if (l.getIsReject())
	{
		return;
	}
	std::string account;
	std::string pwd;
	std::string message;
	l.getAccountPwd(account, pwd);
	int c = loginbili.loginBiliPwd(account, pwd, message);
	if (c == 0)
	{
		ui.pBtLoginAccount->setText("bilibili已登录");
		l.ClearInputBox();
		QString QName = QString::fromStdString(loginbili.getUName());
		ui.lineEditUname->setText(QName);
	}
	else
	{
		QString Qmessage = QString::fromLocal8Bit(message);
		std::wstring wlpstr = Qmessage.toStdWString();
		LPCWSTR lpcWStr = wlpstr.c_str();
		MessageBoxW(NULL, lpcWStr, L"bilibili登录失败", NULL);
		l.ClearInputBox();
	}
}

void BH3ScannerGui::pBtstartScreen()
{
	if (t1.isRunning())
	{
		t1.isExit = true;
		ui.pBtstartScreen->setText("开始监视屏幕");
		return;
	}
	if (t3.isRunning())
	{
		t3.stop();
		t2.stopDownload();
		ui.pBtStream->setText("开始监视直播间");
	}
	//选择和检查账号可用性
	std::string uName;
	if (loginbili.loginBiliKey(uName) != 0)
	{
		QMessageBox msgBox(QMessageBox::Information,
			"提示",
			"登录状态失效，\n请重新登录账号！",
			QMessageBox::Yes,
			this);
		msgBox.exec();
		return;
	}
	t1.biliInitScreen(loginbili.uid, loginbili.access_key, uName);
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
		ui.pBtstartScreen->setText("开始监视屏幕");
		ui.pBtStream->setText("开始监视直播间");
		msgBox.exec();
	}
	else
	{
		QMessageBox msgBox(QMessageBox::Information,
			"提示",
			"扫码失败！",
			QMessageBox::Yes,
			this);
		ui.pBtstartScreen->setText("开始监视屏幕");
		ui.pBtStream->setText("开始监视直播间");
		msgBox.exec();
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
	if (t1.isExit == false)
	{
		t1.isExit = true;
		ui.pBtstartScreen->setText("开始监视屏幕");
	}
	if (t2.isRunning())
	{
		t3.stop();
		t2.stopDownload();
		ui.pBtStream->setText("开始监视直播间");
		return;
	}
	t2.restartDownload();
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
	//选择和检查账号可用性
	std::string uName;
	if (loginbili.loginBiliKey(uName) != 0)
	{
		QMessageBox msgBox(QMessageBox::Information,
			"提示",
			"登录状态失效，\n请重新登录账号！",
			QMessageBox::Yes,
			this);
		msgBox.exec();
		return;
	}
	t3.biliInitStream(loginbili.uid, loginbili.access_key, uName);
	t3.start();
}

int BH3ScannerGui::liveIdError(int code)
{
	switch (code)
	{
	case -1:
	{
		QMessageBox type1(QMessageBox::Information, "提示", "直播间不存在!", QMessageBox::Yes, this);
		type1.exec();
	}
		return 0;
	case -2:
	{
		QMessageBox type2(QMessageBox::Information, "提示", "直播间未开播！", QMessageBox::Yes, this);
		type2.exec();
	}
		return 0;
	case -3:
	{
		QMessageBox type3(QMessageBox::Information, "提示", "未知错误!", QMessageBox::Yes, this);
		type3.exec();
	}
		return 0;
	default:
		return code;
	}

}