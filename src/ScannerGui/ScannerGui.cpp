#include "ScannerGui.h"
#include <QMessageBox>
#include <QWindow>
#include <QRegularExpressionValidator>
#include <qtimer.h>
#include "OfficialApi.h"
#include <fstream>
#include <filesystem>
#include <QStringList>

ScannerGui::ScannerGui(QWidget* parent)
	: QMainWindow(parent)
	, t1(this)
	, t2(this)
	, loginbili(parent)
{
	ui.setupUi(this);
	connect(ui.pBtLoginAccount, &QPushButton::clicked, this, &ScannerGui::pBtLoginAccount);
	connect(ui.pBtstartScreen, &QPushButton::clicked, this, &ScannerGui::pBtstartScreen);
	connect(ui.pBtSwitch, &QPushButton::clicked, this, &ScannerGui::pBtSwitch);
	connect(ui.checkBoxAutoScreen, &QCheckBox::stateChanged, this, &ScannerGui::checkBoxAutoScreen);
	connect(ui.checkBoxAutoExit, &QCheckBox::stateChanged, this, &ScannerGui::checkBoxAutoExit);
	connect(ui.pBtStream, &QPushButton::clicked, this, &ScannerGui::pBtStream);

	connect(ui.tableWidget, &QTableWidget::cellClicked, this, &ScannerGui::getInfo);

	connect(&t1, &ThreadGetScreen::loginResults, this, &ScannerGui::islogin);
	connect(&t2, &ThreadStreamProcess::loginSResults, this, &ScannerGui::islogin);
	
	//加载软件设置
	std::string config0 = loadConfig();
	configJson.parse(config0);

	//UI初始化
	ui.lineEditLiveId->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$"), this));
	ui.lineEditLiveId->setClearButtonEnabled(true);
	if (configJson["auto_start"])
	{
		ui.pBtstartScreen->clicked();
		ui.checkBoxAutoScreen->setChecked(true);
	}
	if (configJson["auto_exit"])
	{
		ui.checkBoxAutoExit->setChecked(true);
	}

	//加载用户信息
	loadUserinfo();
	std::string readName;
	int nUserinfo = userInfo["num"];
	ui.tableWidget->setColumnCount(5);
	QStringList header;
	header << "序号" << "UID" << "用户名" << "类型" << "备注";
	ui.tableWidget->setHorizontalHeaderLabels(header);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
	ui.tableWidget->setColumnWidth(0, 35);
	ui.tableWidget->setColumnWidth(1, 100);
	ui.tableWidget->setColumnWidth(2, 100);
	ui.tableWidget->setColumnWidth(3, 100);
	ui.tableWidget->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
	ui.tableWidget->verticalHeader()->setVisible(false);
	ui.tableWidget->horizontalHeader()->setFont(QFont("楷体", 11));
	ui.tableWidget->setAlternatingRowColors(true);

	ui.tableWidget->horizontalHeader()->setStyleSheet(\
		"QHeaderView::section {"\
		"padding: 1px;"\
		"border: none;"\
		"border-bottom: 1px solid rgb(75, 120, 154);"\
		"border-right: 1px solid rgb(75, 120, 154);"\
		"background-color:#e2e6e7;"\
		"color:#333333;"\
		"}"\
	);

	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

	ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
	
	for (int i = 0; i < nUserinfo;i++)
	{
		insertTableItems(
			QString::fromStdString(userInfo["account"][i]["uid"]),
			QString::fromStdString(userInfo["account"][i]["realname"]),
			"星穹铁道B服",
			"测试中文数据");
	}
	loginbili.openConfig();
	bool repeat = true;
	if (loginbili.loginBiliKey(readName) != 0)
	{
		repeat = false;
	}
	else
	{
		ui.pBtLoginAccount->setText("bilibili已登录");
		QString uname = QString::fromStdString(readName);
		ui.lineEditUname->setText(uname);
	}
	if (!repeat)
	{
		QTimer::singleShot(0, this, SLOT(failure()));
	}

	//ui.tableWidget->setRowCount(16);
	//QTableWidgetItem* item[2];
	//item[0] = new QTableWidgetItem(QString("%1").arg(99));
	//item[1] = new QTableWidgetItem(QString("%1").arg(999));
	//ui.tableWidget->setItem(14, 0, item[0]);
	//ui.tableWidget->setItem(15, 0, item[1]);
	//ui.tableWidget->removeRow(14);
	//ui.tableWidget->removeRow(0);
}

ScannerGui::~ScannerGui()
{

}

void ScannerGui::insertTableItems(QString uid, QString userName, QString type, QString notes)
{
	QTableWidgetItem* item[5];
	int nCount = 0;
	nCount = ui.tableWidget->rowCount();
	ui.tableWidget->insertRow(nCount);
	item[0] = new QTableWidgetItem(QString("%1").arg(nCount + 1));
	ui.tableWidget->setItem(nCount, 0, item[0]);
	item[1] = new QTableWidgetItem(uid);
	ui.tableWidget->setItem(nCount, 1, item[1]);
	item[2] = new QTableWidgetItem(userName);
	ui.tableWidget->setItem(nCount, 2, item[2]);
	item[3] = new QTableWidgetItem(type);
	ui.tableWidget->setItem(nCount, 3, item[3]);
	item[4] = new QTableWidgetItem(notes);
	ui.tableWidget->setItem(nCount, 4, item[4]);
}

void ScannerGui::pBtLoginAccount()
{
	ui.pBtLoginAccount->setEnabled(false);
	ui.pBtstartScreen->setEnabled(false);
	ui.pBtStream->setEnabled(false);
	if (t1.isRunning() || t2.isRunning())
	{
		t1.stop();
		t2.stop();
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

void ScannerGui::pBtstartScreen()
{
	if (t1.isRunning())
	{
		t1.stop();
		ui.pBtstartScreen->setText("开始监视屏幕");
		return;
	}
	if (t2.isRunning())
	{
		t2.stop();
		ui.pBtStream->setText("开始监视直播间");
	}
	std::string uName;
	//检查选择的账号可用性
	if (loginbili.loginBiliKey(uName) != 0)
	{
		failure();
		return;
	}
	int num = 0;
	std::istringstream ss((std::string)userInfo["account"][countA]["uid"]);
	ss >> num;
	t1.InitScreen(num, userInfo["account"][countA]["access_key"], uName);
	ui.pBtstartScreen->setText("监视屏幕二维码中");
	t1.start();
}

void ScannerGui::pBtStream()
{
	if (t1.isRunning())
	{
		t1.stop();
		ui.pBtstartScreen->setText("开始监视屏幕");
	}
	if (t2.isRunning())
	{
		t2.stop();
		ui.pBtStream->setText("开始监视直播间");
		return;
	}
	//选择和检查账号可用性
	std::string uName;
	if (loginbili.loginBiliKey(uName) != 0)
	{
		failure();
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
	t2.url = streamAddress;
	ui.pBtStream->setText("监视直播二维码中");
	t2.biliInitStream(loginbili.uid, loginbili.access_key, uName);
	t2.start();
}

void ScannerGui::closeEvent(QCloseEvent* event)
{
	t1.stop();
	t2.stop();
}

void ScannerGui::showEvent(QShowEvent* event)
{

}

void ScannerGui::islogin(const bool& b)
{
	t1.stop();
	t2.stop();
	if (b)
	{
		if (configJson["auto_exit"])
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

void ScannerGui::checkBoxAutoScreen(int state)
{
	if (state == Qt::Checked)
	{
		configJson["auto_start"] = true;
		updateConfig0();
	}
	else if (state == Qt::Unchecked)
	{
		configJson["auto_start"] = false;
		updateConfig0();
	}
}

void ScannerGui::checkBoxAutoExit(int state)
{
	if (state == Qt::Checked)
	{
		configJson["auto_exit"] = true;
		updateConfig0();
	}
	else if (state == Qt::Unchecked)
	{
		configJson["auto_exit"] = false;
		updateConfig0();
	}
}

void ScannerGui::updateConfig0()//先放这里
{
	const std::string output = userInfo.str();
	std::ofstream outFile("./Config/user2.json");
	std::stringstream outStr;
	bool isInPair = false;
	for (int i = 0; i < output.size(); i++)
	{
		if (output[i] == '{')
		{
			outStr << "{\n";
			continue;
		}
		if (output[i] == '}')
		{
			outStr << "\n}";
			isInPair = false;
			continue;
		}
		if (output[i] == ',')
		{
			outStr << ",\n";
			isInPair = false;
			continue;
		}
		if (!isInPair)
		{
			outStr << "  ";
			isInPair = true;
		}
		outStr << output[i];
	}
	outFile << outStr.str();
	outFile.close();
}

void ScannerGui::loadUserinfo()
{
	std::vector<std::string> files;
	const std::string& filePath = "./Config/user1.json";
	std::string configContent = readConfigFile(filePath);
	userInfo.parse(configContent);
}


int ScannerGui::liveIdError(int code)
{
	switch (code)
	{
	case -1:
	{
		QMessageBox::information(this, "提示", "直播间不存在!", QMessageBox::Yes);

	}
	return 0;
	case -2:
	{
		QMessageBox::information(this, "提示", "直播间未开播！", QMessageBox::Yes);

	}
	return 0;
	case -3:
	{
		QMessageBox::information(this, "提示", "未知错误!", QMessageBox::Yes);
	}
	return 0;
	default:
		return code;
	}
}

std::string ScannerGui::loadConfig()
{
	const std::string filePath = "./Config/config0.json";
	//检查路径是否存在。
	if (std::filesystem::exists(filePath))
	{
		// 文件存在，读取配置
		std::string configContent = readConfigFile(filePath);
		return configContent;
	}
	else
	{
		// 默认值
		std::string defaultConfig =
		R"({
	"auto_exit": false,
	"auto_start": false
})";
		// 文件不存在，创建默认配置文件
		createDefaultConfigFile(filePath, defaultConfig);
		return defaultConfig;
	}

}
std::string ScannerGui::readConfigFile(const std::string& filePath)
{
	std::ifstream file(filePath);
	if (file.is_open())
	{
		// 将文件内容读取到字符串
		std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		file.close();
		std::cout << "读取配置文件成功。\n";
		return content;
	}
	else
	{
		std::cout << "无法打开配置文件。\n";
		return "";
	}
}

void ScannerGui::createDefaultConfigFile(const std::string& filePath, std::string defaultConfig)
{
	std::ofstream file(filePath);
	if (file.is_open())
	{
		file << defaultConfig;
		file.close();
		std::cout << "创建并写入默认配置文件成功。\n";
	}
	else
	{
		std::cout << "无法创建配置文件。\n";
	}
}

void ScannerGui::failure()
{
	QMessageBox* messageBox = new QMessageBox(this);
	messageBox->setAttribute(Qt::WA_DeleteOnClose);
	messageBox->setText("登录状态失效，\n请重新添加账号！");
	messageBox->setWindowTitle("提示");
	messageBox->setIcon(QMessageBox::Information);
	messageBox->show();
}

void ScannerGui::getInfo(int x, int y)
{
	QTableWidgetItem* item = ui.tableWidget->item(x, y - 1);//溢出
	QString cellText = item->text();
	countA = x;
	HttpClient h;
	std::cout << "x=" << x << "y=" << y << " " << h.UTF8_To_string(cellText.toStdString()) << std::endl;
}

void ScannerGui::pBtSwitch()
{
	QTableWidgetItem* item = ui.tableWidget->item(countA, countA);
	item->setSelected(false);
}
