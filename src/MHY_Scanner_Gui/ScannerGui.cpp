#include "ScannerGui.h"

#include <fstream>
#include <filesystem>

#include <QMessageBox>
#include <QWindow>
#include <QRegularExpressionValidator>
#include <QStringList> 
#include <Json.h>
#include <trrlog.hpp>

#include "OfficialApi.h"
#include "Mihoyosdk.h"
#include "LoginBili.h"
#include "LoginWindow.h"

ScannerGui::ScannerGui(QWidget* parent)
	: QMainWindow(parent)
	, t1(this)
	, t2(this)
{
	ui.setupUi(this);
	connect(ui.action1_3, &QAction::triggered, this, &ScannerGui::LoginAccount);
	connect(ui.action1_4, &QAction::triggered, this, &ScannerGui::DeleteAccount);
	connect(ui.action2_3, &QAction::triggered, this, &ScannerGui::SetDefaultAccount);
	connect(ui.action1_2, &QAction::triggered, this, &ScannerGui::About);
	connect(ui.action2_2, &QAction::triggered, this, &ScannerGui::help);

	connect(ui.pBtstartScreen, &QPushButton::clicked, this, &ScannerGui::pBtstartScreen);
	connect(ui.pBtStop, &QPushButton::clicked, this, &ScannerGui::pBtStop);
	connect(ui.checkBoxAutoScreen, &QCheckBox::clicked, this, &ScannerGui::checkBoxAutoScreen);
	connect(ui.checkBoxAutoExit, &QCheckBox::clicked, this, &ScannerGui::checkBoxAutoExit);
	connect(ui.checkBoxAutoLogin, &QCheckBox::clicked, this, &ScannerGui::checkBoxAutoLogin);
	connect(ui.pBtStream, &QPushButton::clicked, this, &ScannerGui::pBtStream);
	connect(ui.tableWidget, &QTableWidget::cellClicked, this, &ScannerGui::getInfo);

	connect(&t1, &QRCodeForScreen::loginResults, this, &ScannerGui::islogin);
	connect(&t1, &QRCodeForScreen::loginConfirm, this, &ScannerGui::loginConfirmTip);
	connect(&t2, &ThreadStreamProcess::loginResults, this, &ScannerGui::islogin);
	connect(&t2, &ThreadStreamProcess::loginConfirm, this, &ScannerGui::loginConfirmTip);
	connect(&configinitload, &configInitLoad::userinfoTrue, this, &ScannerGui::configInitUpdate);

	o.start();
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

	ui.tableWidget->horizontalHeader()->setStyleSheet(
		"QHeaderView::section {"
		"padding: 1px;"
		"border: none;"
		"border-bottom: 1px solid rgb(75, 120, 154);"
		"border-right: 1px solid rgb(75, 120, 154);"
		"background-color:#e2e6e7;"
		"color:#333333;"
		"}"
	);
	ui.label_3->setText(SCAN_VER);

	ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
	ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
	ui.tableWidget->setEditTriggers(QAbstractItemView::DoubleClicked);

	ui.lineEditLiveId->setValidator(new QRegularExpressionValidator(QRegularExpression("[0-9]+$"), this));
	ui.lineEditLiveId->setClearButtonEnabled(true);

	configinitload.start();

	trrlog::SetLogFile("log.txt");
	trrlog::Log_debug("UI Initialization completed");
}

ScannerGui::~ScannerGui()
{
	t1.stop();
	t2.stop();
}

void ScannerGui::insertTableItems(QString uid, QString userName, QString type, QString notes)
{
	QTableWidgetItem* item[5]{};
	int nCount = ui.tableWidget->rowCount();
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

	for (int i = 0; i < 4; i++)
	{
		QTableWidgetItem* item1 = ui.tableWidget->item(nCount, i);
		item1->setFlags(item1->flags() & ~Qt::ItemIsEditable);
	}
}

void ScannerGui::LoginAccount()
{
	LoginWindow loginwindow(reinterpret_cast<QDialog*>(this));
	if (t1.isRunning() || t2.isRunning())
	{
		QMessageBox::information(this, "错误", "请先停止识别！", QMessageBox::Yes);
		return;
	}
	loginwindow.exec();
	if (loginwindow.getIsReject())
	{
		return;
	}
	if (loginwindow.type == 1)
	{
		OfficialApi o;
		if (o.cookieParser(loginwindow.cookie) != 0)
		{
			QMessageBox::information(this, "错误", "cookie格式错误", QMessageBox::Yes);
			return;
		}
		std::string uid = o.getUid();
		if (checkDuplicates(uid))
		{
			QMessageBox::information(this, "提示", "该账号已添加，无需重复添加", QMessageBox::Yes);
			return;
		}
		std::string token;
		int code = o.getMultiTokenByLoginTicket(token);
		if (code == 0)
		{
			std::string name = o.getUserName(uid);

			int num = userinfo["num"];
			insertTableItems(QString::fromStdString(uid), QString::fromStdString(name), "官服", "");
			userinfo["account"][num]["access_key"] = token;
			userinfo["account"][num]["uid"] = uid;
			userinfo["account"][num]["name"] = name;
			userinfo["account"][num]["type"] = "官服";
			userinfo["account"][num]["note"] = "";
			userinfo["num"] = num + 1;
		}
		else
		{
			QString info = QString::fromStdString(token);
			QMessageBox::information(this, "错误", info, QMessageBox::Yes);
		}
	}
	if (loginwindow.type == 2)
	{
		LoginBili loginbili;
		std::string account;
		std::string pwd;
		std::string message;
		std::string name;
		std::string uid;
		std::string token;
		loginwindow.getAccountPwd(account, pwd);
		int code = loginbili.loginBiliPwd(account, pwd, message, uid, token, name);
		if (checkDuplicates(uid))
		{
			QMessageBox::information(this, "提示", "该账号已添加，无需重复添加", QMessageBox::Yes);
			return;
		}
		if (code != 0)
		{
			QMessageBox::information(this, "提示", QString::fromStdString(message), QMessageBox::Yes);
		}
		else
		{
			int num = userinfo["num"];
			insertTableItems(QString::fromStdString(uid), QString::fromStdString(name), "崩坏3B服", "");
			userinfo["account"][num]["access_key"] = token;
			userinfo["account"][num]["uid"] = uid;
			userinfo["account"][num]["name"] = name;
			userinfo["account"][num]["type"] = "崩坏3B服";
			userinfo["account"][num]["note"] = "";
			userinfo["num"] = num + 1;
		}
	}
	m_config->updateConfig(userinfo.str());
}

void ScannerGui::pBtstartScreen()
{
	ui.pBtstartScreen->setEnabled(false);
	ui.pBtStream->setEnabled(false);

	if (countA == -1)
	{
		QMessageBox::information(this, "提示", "没有选择任何账号", QMessageBox::Yes);
		goto exit;
	}
	if (std::string type = userinfo["account"][countA]["type"]; type == "官服")
	{
		OfficialApi o;
		std::string stoken = userinfo["account"][countA]["access_key"];
		std::string uid = userinfo["account"][countA]["uid"];
		std::string gameToken;
		//可用性检查
		int code = o.getGameToken(stoken, uid, gameToken);
		if (code != 0)
		{
			failure();
			goto exit;
		}
		t1.setServerType(ServerType::Type::Official);
		t1.setLoginInfo(uid, gameToken);
	}
	else if (type == "崩坏3B服")
	{
		LoginBili b;
		std::string stoken = userinfo["account"][countA]["access_key"];
		std::string uid = userinfo["account"][countA]["uid"];
		std::string name;
		//可用性检查
		int code = b.loginBiliKey(name, uid, stoken);
		if (code != 0)
		{
			failure();
			goto exit;
		}
		t1.setServerType(ServerType::Type::BH3_BiliBili);
		t1.setLoginInfo(uid, stoken, name);
	}
	else
	{
		goto exit;
	}
	t1.start();
	ui.pBtstartScreen->setText("监视屏幕中");
	return;
exit:
	ui.pBtstartScreen->setEnabled(true);
	ui.pBtStream->setEnabled(true);
	return;
}

void ScannerGui::pBtStream()
{
	ui.pBtstartScreen->setEnabled(false);
	ui.pBtStream->setEnabled(false);

	std::string stream_link;
	std::map<std::string, std::string> heards;
	if (countA == -1)
	{
		QMessageBox::information(this, "提示", "没有选择任何账号", QMessageBox::Yes);
		goto exit;
	}
	//检查直播间状态
	if (!getStreamLink(ui.lineEditLiveId->text().toStdString(), stream_link, heards))
	{
		goto exit;
	}
	else
	{
		t2.setUrl(stream_link, heards);
	}
	if (const std::string& type = userinfo["account"][countA]["type"]; type == "官服")
	{
		OfficialApi o;
		std::string stoken = userinfo["account"][countA]["access_key"];
		std::string uid = userinfo["account"][countA]["uid"];
		std::string gameToken;
		//可用性检查
		int code = o.getGameToken(stoken, uid, gameToken);
		if (code != 0)
		{
			failure();
			goto exit;
		}
		t2.setServerType(ServerType::Type::Official);
		t2.setLoginInfo(uid, gameToken);
	}
	else if (type == "崩坏3B服")
	{
		LoginBili b;
		std::string stoken = userinfo["account"][countA]["access_key"];
		std::string uid = userinfo["account"][countA]["uid"];
		std::string name;
		//可用性检查
		int code = b.loginBiliKey(name, uid, stoken);
		if (code != 0)
		{
			failure();
			goto exit;
		}
		t2.setServerType(ServerType::Type::BH3_BiliBili);
		t2.setLoginInfo(uid, stoken, name);
	}
	else
	{
		goto exit;
	}
	t2.start(QThread::Priority::TimeCriticalPriority);
	ui.pBtStream->setText("监视直播中");
	return;
exit:
	ui.pBtstartScreen->setEnabled(true);
	ui.pBtStream->setEnabled(true);
	return;
}

void ScannerGui::closeEvent(QCloseEvent* event)
{
	t1.stop();
	t2.stop();
}

void ScannerGui::showEvent(QShowEvent* event)
{

}

void ScannerGui::islogin(const ScanRet::Type ret)
{
	QMessageBox* messageBox = new QMessageBox(this);
	auto Show_QMessageBox = [&](const QString& title, const QString& text)
		{
			ui.pBtStream->setText("监视直播间");
			ui.pBtstartScreen->setText("监视屏幕");
			messageBox->setIcon(QMessageBox::Information);
			messageBox->setWindowTitle(title);
			messageBox->setText(text);
			messageBox->addButton(QMessageBox::Yes);
			messageBox->show();
		};
	switch (ret)
	{
	case ScanRet::UNKNOW:
		break;
	case ScanRet::FAILURE_1:
		Show_QMessageBox("提示", "扫码失败!");
		break;
	case ScanRet::FAILURE_2:
		Show_QMessageBox("提示", "扫码二次确认失败!");
		break;
	case ScanRet::LIVESTOP:
		Show_QMessageBox("提示", "直播中断!");
		break;
	case ScanRet::SUCCESS:
	{
		Show_QMessageBox("提示", "扫码成功!");
		if (userinfo["auto_exit"])
		{
			exit(0);
		}
		break;
	}
	case ScanRet::STREAMERROR:
		Show_QMessageBox("提示", "直播流初始化失败!");
		break;
	default:
		break;
	}
}

void ScannerGui::loginConfirmTip(const GameType::Type gameType, bool b)
{
	QString info("将用账号" + ui.lineEditUname->text());
	switch (gameType)
	{
	case GameType::Type::Honkai3:
		info += "\n登录崩坏3\n";
		break;
	case GameType::Type::Honkai3_BiliBili:
		info += "\n登录BiliBili崩坏3\n";
		break;
	case GameType::Type::Genshin:
		info += "\n登录原神\n";
		break;
	case GameType::Type::StarRail:
		info += "\n登录星穹铁道\n";
		break;
	default:
		break;
	}
	QMessageBox::StandardButton result = QMessageBox::information(this, "提示", info + "确认登录？", QMessageBox::Yes | QMessageBox::No);
	if (result != QMessageBox::Yes)
	{
		ui.pBtStream->setText("监视直播间");
		ui.pBtstartScreen->setText("监视屏幕");
		return;
	}
	if (b)
	{
		t1.continueLastLogin();
	}
	else
	{
		t2.continueLastLogin();
	}
}

void ScannerGui::checkBoxAutoScreen(bool clicked)
{
	int state = ui.checkBoxAutoScreen->checkState();
	if ((int)userinfo["last_account"] == 0)
	{
		ui.checkBoxAutoScreen->setChecked(false);
		QMessageBox::information(this, "提示", "你没有选择默认账号!", QMessageBox::Yes);
		return;
	}
	if (state == Qt::Checked)
	{
		ui.checkBoxAutoScreen->setChecked(true);
		userinfo["auto_start"] = true;
	}
	else if (state == Qt::Unchecked)
	{
		ui.checkBoxAutoScreen->setChecked(false);
		userinfo["auto_start"] = false;
	}
	m_config->updateConfig(userinfo.str());
}

void ScannerGui::checkBoxAutoExit(bool clicked)
{
	int state = ui.checkBoxAutoExit->checkState();
	if (state == Qt::Checked)
	{
		userinfo["auto_exit"] = true;
	}
	else if (state == Qt::Unchecked)
	{
		userinfo["auto_exit"] = false;
	}
	m_config->updateConfig(userinfo.str());
}

void ScannerGui::checkBoxAutoLogin(bool clicked)
{
	int state = ui.checkBoxAutoLogin->checkState();
	if (state == Qt::Checked)
	{
		userinfo["auto_login"] = true;
	}
	else if (state == Qt::Unchecked)
	{
		userinfo["auto_login"] = false;
	}
	m_config->updateConfig(userinfo.str());
}

int ScannerGui::liveIdError(const LiveStreamStatus::Status data)
{
	switch (data)
	{
	case LiveStreamStatus::Absent:
	{
		QMessageBox::information(this, "提示", "直播间不存在!", QMessageBox::Yes);
		return 1;
	}
	case LiveStreamStatus::NotLive:
	{
		QMessageBox::information(this, "提示", "直播间未开播！", QMessageBox::Yes);
		return 1;
	}
	case LiveStreamStatus::Error:
	{
		QMessageBox::information(this, "提示", "直播间未知错误!", QMessageBox::Yes);
		return 1;
	}
	default:
		return 0;
	}
	return 0;
}


int ScannerGui::getSelectedRowIndex()
{
	QList<QTableWidgetItem*> item = ui.tableWidget->selectedItems();
	if (item.count() == 0)
	{
		return -1;
	}
	return ui.tableWidget->row(item.at(0));
}

bool ScannerGui::checkDuplicates(const std::string uid)
{
	for (int i = 0; i < (int)userinfo["num"]; i++)
	{
		std::string m_uid = userinfo["account"][i]["uid"];
		if (uid == m_uid)
		{
			return true;
		}
	}
	return false;
}

bool ScannerGui::getStreamLink(const std::string& roomid, std::string& url, std::map<std::string, std::string>& heards)
{
	std::uint32_t live_type = ui.comboBox->currentIndex();
	switch (live_type)
	{
	case 0:
	{
		LiveBili live(roomid);
		if (liveIdError(live.GetLiveStreamStatus()) != 0)
		{
			return false;
		}
		heards =
		{
			{"user_agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) \
			Chrome/110.0.0.0 Safari/537.36 Edg/110.0.1587.41"},
			{"referer", "https://live.bilibili.com"}
		};
		url = live.GetLiveStreamLink();
		break;

	}
	case 1:
	{
		LiveDouyin live(roomid);
		if (liveIdError(live.GetLiveStreamStatus()) != 0)
		{
			return false;
		}
		url = live.GetLiveStreamLink();
		break;
	}
	case 2:
	{
		LiveHuya live(roomid);
		if (liveIdError(live.GetLiveStreamStatus()) != 0)
		{
			return false;
		}
		url = live.GetLiveStreamLink();
		break;
	}
	default:
		break;
	}
	return true;
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
	QTableWidgetItem* item = ui.tableWidget->item(x, 2);
	QString cellText = item->text();
	ui.lineEditUname->setText(cellText);
	countA = x;
	std::string s = std::format(R"(row = {} , user_name = {})", x, cellText.toStdString());
	trrlog::Log_debug("{}", s);
}

void ScannerGui::SetDefaultAccount()
{
	int nCurrentRow = getSelectedRowIndex();
	if (nCurrentRow != -1)
	{
		//ui.tableWidget->setCurrentCell(nCurrentRow, QItemSelectionModel::Current);
		userinfo["last_account"] = nCurrentRow + 1;
		m_config->updateConfig(userinfo.str());
		QMessageBox::information(this, "设置成功！", "勾选下方\"启动时自动监视屏幕\"将在下次启动时自动扫描并使用该账号登录", QMessageBox::Yes);
		return;
	}
	else
	{
		QMessageBox::information(this, "提示", "没有选择任何账号", QMessageBox::Yes);
		return;
	}
}

void ScannerGui::DeleteAccount()
{
	int nCurrentRow = getSelectedRowIndex();
	if (nCurrentRow == -1)
	{
		QMessageBox::information(this, "提示", "没有选择任何账号", QMessageBox::Yes);
		return;
	}
	if (int re = QMessageBox::information(this, "删除确认",
		QString::fromStdString(std::format("你正在删除账号\n{}", (std::string)userinfo["account"][countA]["name"])),
		QMessageBox::Yes | QMessageBox::No);
		re != QMessageBox::Yes)
	{
		return;
	}
	userinfo["num"] = (int)userinfo["num"] - 1;
	//判断删除的账号是否是启动默认账号
	if (static_cast<int>(userinfo["last_account"]) == countA + 1)
	{
		userinfo["last_account"] = 0;
	}
	userinfo["account"].remove(countA);
	const std::string& str = userinfo.str();
	trrlog::Log_debug("{}", str);
	m_config->updateConfig(userinfo.str());
	ui.tableWidget->setCurrentCell(nCurrentRow, QItemSelectionModel::Current);
	ui.tableWidget->removeRow(nCurrentRow);
	for (int i = 0; i < (int)userinfo["num"]; i++)
	{
		QTableWidgetItem* item = new QTableWidgetItem(QString("%1").arg(i + 1));
		ui.tableWidget->setItem(i, 0, item);
	}
}

void ScannerGui::About()
{
	AboutDialog aboutDialog(reinterpret_cast<QDialog*>(this));
	aboutDialog.exec();
}

void ScannerGui::help()
{
	ShellExecuteW(NULL, L"open", L"https://github.com/Theresa-0328/MHY_Scanner/issues", NULL, NULL, SW_SHOWNORMAL);
}

void ScannerGui::pBtStop()
{
	t1.stop();
	t2.stop();
	ui.pBtstartScreen->setText("监视屏幕");
	ui.pBtStream->setText("监视直播间");
	ui.pBtstartScreen->setEnabled(true);
	ui.pBtStream->setEnabled(true);
}

void ScannerGui::configInitUpdate(bool b)
{
	if (!b)
	{
		int result = QMessageBox::information(this, "错误", "配置文件错误！\n重置配置文件为默认？", QMessageBox::Yes | QMessageBox::No);
		if (result == QMessageBox::Yes)
		{
			m_config->defaultConfig();
		}
		else
		{
			QMessageBox::information(this, "错误", "配置文件错误！\n无法继续运行！", QMessageBox::Yes);
			exit(1);
		}
	}
	userinfo.parse(m_config->getConfig());
	for (int i = 0; i < (int)userinfo["num"]; i++)
	{
		insertTableItems(
			QString::fromStdString(userinfo["account"][i]["uid"]),
			QString::fromStdString(userinfo["account"][i]["name"]),
			QString::fromStdString(userinfo["account"][i]["type"]),
			QString::fromStdString(userinfo["account"][i]["note"]));
	}
	if (userinfo["auto_start"] && static_cast<int>(userinfo["last_account"]) != 0)
	{
		countA = static_cast<int>(userinfo["last_account"]) - 1;
		ui.pBtstartScreen->clicked();
		ui.checkBoxAutoScreen->setChecked(true);
		ui.lineEditUname->setText(QString::fromStdString(userinfo["account"][countA]["name"]));
		ui.tableWidget->setCurrentCell(countA, QItemSelectionModel::Select);
	}
	if (userinfo["auto_exit"])
	{
		ui.checkBoxAutoExit->setChecked(true);
	}
	if (userinfo["auto_login"])
	{
		ui.checkBoxAutoLogin->setChecked(true);
	}
	connect(ui.tableWidget, &QTableWidget::itemChanged, this, &ScannerGui::updateNote);
}

void ScannerGui::updateNote(QTableWidgetItem* item)
{
	QString text = item->text();
	userinfo["account"][item->row()]["note"] = text.toStdString();
	m_config->updateConfig(userinfo.str());
}

void OnlineUpdate::run()
{
	Mihoyosdk m;
	m.setOAServer();
}

OnlineUpdate::~OnlineUpdate()
{
	wait();
}

void configInitLoad::run()
{
	const std::string& config = m_config->getConfig();
	json::Json data;
	try
	{
		data.parse(config);
		int num = data["num"];
		for (int i = 0; i < num; i++)
		{
			std::string s = data["account"][i]["uid"];
			s = data["account"][i]["name"];
			s = data["account"][i]["type"];
		};
		int i = data["last_account"];
		bool b = data["auto_start"];
		b = data["auto_login"];
		b = data["auto_exit"];

		//兼容旧配置文件，将在后续移除
		for (int i = 0; i < (int)data["num"]; i++)
		{
			try
			{
				const std::string& str = data["account"][i]["note"];
			}
			catch (...)
			{
				data["account"][i]["note"] = "";
			}
		}
		m_config->updateConfig(data.str());

		emit userinfoTrue(true);
	}
	catch (...)
	{
		emit userinfoTrue(false);
	}
}

configInitLoad::~configInitLoad()
{
	wait();
}
