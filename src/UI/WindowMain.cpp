#include "WindowMain.h"

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
#include "MhyApi.hpp"
#include "LoginBili.h"
#include "LoginWindow.h"

WindowMain::WindowMain(QWidget* parent) :
    QMainWindow(parent),
    t1(this),
    t2(this)
{
    ui.setupUi(this);
    connect(ui.action1_3, &QAction::triggered, this, &WindowMain::AddAccount);
    connect(ui.action1_4, &QAction::triggered, this, &WindowMain::SetDefaultAccount);
    connect(ui.action2_3, &QAction::triggered, this, &WindowMain::DeleteAccount);
    connect(ui.action1_2, &QAction::triggered, this, [this]() {
        WindowAbout WindowAbout(this);
        WindowAbout.exec();
    });
    connect(ui.action2_2, &QAction::triggered, this, []() {
        ShellExecuteW(NULL, L"open", L"https://github.com/Theresa-0328/MHY_Scanner/issues", NULL, NULL, SW_SHOWNORMAL);
    });
    connect(ui.action1_5, &QAction::triggered, this, []() {
        ShellExecuteW(NULL, L"open", L"config", NULL, NULL, SW_SHOWDEFAULT);
    });
    connect(ui.pBtstartScreen, &QPushButton::clicked, this, &WindowMain::pBtstartScreen);
    connect(this, &WindowMain::StopScanner, this, &WindowMain::pBtStop);
    connect(this, &WindowMain::StartScanScreen, this, [&]() {
        ui.pBtstartScreen->setText("监视屏幕中");
        ui.pBtstartScreen->setEnabled(true);
    });
    connect(this, &WindowMain::AccountError, this, [&]() {
        failure();
        pBtStop();
    });
    connect(this, &WindowMain::StartScanLive, this, [&]() {
        ui.pBtStream->setText("监视直播中");
        ui.pBtStream->setEnabled(true);
    });
    connect(this, &WindowMain::LiveStreamLinkError, this, [&](LiveStreamStatus status) {
        liveIdError(status);
    });
    connect(this, &WindowMain::AccountNotSelected, this, [&]() {
        QMessageBox::information(this, "提示", "没有选择任何账号", QMessageBox::Yes);
        pBtStop();
    });
    connect(ui.checkBoxAutoScreen, &QCheckBox::clicked, this, &WindowMain::checkBoxAutoScreen);
    connect(ui.checkBoxAutoExit, &QCheckBox::clicked, this, &WindowMain::checkBoxAutoExit);
    connect(ui.checkBoxAutoLogin, &QCheckBox::clicked, this, &WindowMain::checkBoxAutoLogin);
    connect(ui.pBtStream, &QPushButton::clicked, this, &WindowMain::pBtStream);
    connect(ui.tableWidget, &QTableWidget::cellClicked, this, &WindowMain::getInfo);
    connect(&t1, &QRCodeForScreen::loginResults, this, &WindowMain::islogin);
    connect(&t1, &QRCodeForScreen::loginConfirm, this, &WindowMain::loginConfirmTip);
    connect(&t2, &QRCodeForStream::loginResults, this, &WindowMain::islogin);
    connect(&t2, &QRCodeForStream::loginConfirm, this, &WindowMain::loginConfirmTip);
    connect(&configinitload, &configInitLoad::userinfoTrue, this, &WindowMain::configInitUpdate);
    connect(ui.tableWidget, &QTableWidget::itemChanged, this, &WindowMain::updateNote);

    QThreadPool::globalInstance()->setMaxThreadCount(QThread::idealThreadCount());
    o.start();
    ui.tableWidget->setColumnCount(5);
    QStringList header;
    header << "序号"
           << "UID"
           << "用户名"
           << "类型"
           << "备注";
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
        "}");
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

WindowMain::~WindowMain()
{
    t1.stop();
    t2.stop();
}

void WindowMain::insertTableItems(QString uid, QString userName, QString type, QString notes)
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

void WindowMain::AddAccount()
{
#if 1
    if (t1.isRunning() || t2.isRunning())
    {
        QMessageBox::information(this, "错误", "请先停止识别！", QMessageBox::Yes);
        return;
    }
    m_windowLogin = new WindowLogin(this);
    connect(m_windowLogin, &WindowLogin::AddUserInfo, this, [this](const std::string& name, const std::string& V2Token, const std::string& uid, const std::string& mid, const std::string& type) {
        if (checkDuplicates(uid.data()))
        {
            QMessageBox::information(this, "提示", "该账号已添加，无需重复添加", QMessageBox::Yes);
            return;
        }
        //TODO 有预期外信号触发,潜在bug
        insertTableItems(QString::fromStdString(uid), QString::fromStdString(name), QString::fromStdString(type), "");
        QThreadPool::globalInstance()->start([this, V2Token, uid, name, type, mid] {
            int num{ userinfo["num"] };
            userinfo["account"][num]["access_key"] = V2Token;
            userinfo["account"][num]["uid"] = uid;
            userinfo["account"][num]["name"] = name;
            userinfo["account"][num]["type"] = type;
            userinfo["account"][num]["note"] = "";
            userinfo["account"][num]["mid"] = mid;
            userinfo["num"] = num + 1;
            m_config->updateConfig(userinfo.str());
        });
        QMessageBox::information(this, "提示", "添加成功", QMessageBox::Yes);
    });
    connect(m_windowLogin, &WindowLogin::Destroy, this, [this] {
        m_windowLogin->deleteLater();
    });
    m_windowLogin->show();
#else
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
        if (o.cookieParser(loginwindow.cookie) != true)
        {
            QMessageBox::information(this, "错误", "cookie格式错误", QMessageBox::Yes);
            return;
        }
        std::string uid{ o.getUid() };
        if (checkDuplicates(uid))
        {
            QMessageBox::information(this, "提示", "该账号已添加，无需重复添加", QMessageBox::Yes);
            return;
        }
        std::string stoken{ o.getStoken() };
        std::string mid{ o.getMid() };
        std::string gameToken{};
        int code = o.getGameToken(stoken, mid, gameToken);
        if (code == 0)
        {
            std::string name{ o.getUserName(uid) };
            int num = userinfo["num"];
            insertTableItems(QString::fromStdString(uid), QString::fromStdString(name), "官服", "");
            userinfo["account"][num]["access_key"] = stoken;
            userinfo["account"][num]["uid"] = uid;
            userinfo["account"][num]["name"] = name;
            userinfo["account"][num]["type"] = "官服";
            userinfo["account"][num]["note"] = "";
            userinfo["account"][num]["mid"] = mid;
            userinfo["num"] = num + 1;
        }
        else
        {
            QString info = QString::fromStdString("验证失败,cookie可能过期");
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
        if (code == 500002)
        {
            QMessageBox::information(this, "错误", "账号或密码错误，请重新输入", QMessageBox::Yes);
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
            userinfo["account"][num]["mid"] = "";
            userinfo["num"] = num + 1;
        }
    }
    if (loginwindow.type == 3)
    {
        std::string pwd;
        std::string uid;
        loginwindow.getAccountPwd(uid, pwd);
        if (checkDuplicates(uid))
        {
            QMessageBox::information(this, "提示", "该账号已添加，无需重复添加", QMessageBox::Yes);
            return;
        }
        std::string name{ getMysUserName(uid) };
        int num = userinfo["num"];
        insertTableItems(QString::fromStdString(uid), QString::fromStdString(name), "官服", "");
        const auto result = getStokenByGameToken(uid, pwd);
        if (!result)
        {
            return;
        }
        const auto& [mid, key] = *result;
        userinfo["account"][num]["access_key"] = key;
        userinfo["account"][num]["uid"] = uid;
        userinfo["account"][num]["name"] = name;
        userinfo["account"][num]["type"] = "官服";
        userinfo["account"][num]["note"] = "";
        userinfo["account"][num]["mid"] = mid;
        userinfo["num"] = num + 1;
    }
    m_config->updateConfig(userinfo.str());
#endif // 0
}

void WindowMain::pBtstartScreen(bool clicked)
{
    ui.pBtstartScreen->setEnabled(false);
    ui.pBtStream->setEnabled(false);
    ui.pBtstartScreen->setText("加载中。。。");
    QApplication::processEvents();
    QThreadPool::globalInstance()->start([&, clicked]() {
        if (!clicked)
        {
            emit StopScanner();
            return;
        }
        //FIXME 没有及时更新
        if (countA == -1)
        {
            emit AccountNotSelected();
            return;
        }
        if (std::string type = userinfo["account"][countA]["type"]; type == "官服")
        {
            OfficialApi o;
            std::string stoken = userinfo["account"][countA]["access_key"];
            std::string uid = userinfo["account"][countA]["uid"];
            std::string mid = userinfo["account"][countA]["mid"];
            std::string gameToken;
            //可用性检查
            int code = o.getGameToken(stoken, mid, gameToken);
            if (code != 0)
            {
                emit AccountError();
                return;
            }
            t1.setServerType(ServerType::Official);
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
                emit AccountError();
                return;
            }
            t1.setServerType(ServerType::BH3_BiliBili);
            t1.setLoginInfo(uid, stoken, name);
        }
        t1.start();
        emit StartScanScreen();
    });
}

void WindowMain::pBtStream(bool clicked)
{
    ui.pBtstartScreen->setEnabled(false);
    ui.pBtStream->setEnabled(false);
    ui.pBtStream->setText("加载中。。。");
    QApplication::processEvents();

    QThreadPool::globalInstance()->start([&, clicked]() {
        if (!clicked)
        {
            emit StopScanner();
            return;
        }
        if (countA == -1)
        {
            emit AccountNotSelected();
            return;
        }
        std::string stream_link;
        std::map<std::string, std::string> heards;
        //检查直播间状态
        if (!GetStreamLink(ui.lineEditLiveId->text().toStdString(), stream_link, heards))
        {
            emit StopScanner();
            return;
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
            std::string mid = userinfo["account"][countA]["mid"];
            std::string gameToken;
            //可用性检查
            int code = o.getGameToken(stoken, mid, gameToken);
            if (code != 0)
            {
                emit AccountError();
                return;
            }
            t2.setServerType(ServerType::Official);
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
                emit AccountError();
                return;
            }
            t2.setServerType(ServerType::BH3_BiliBili);
            t2.setLoginInfo(uid, stoken, name);
        }
        t2.start();
        emit StartScanLive();
    });
}

void WindowMain::closeEvent(QCloseEvent* event)
{
    t1.stop();
    t2.stop();
}

void WindowMain::showEvent(QShowEvent* event)
{
}

void WindowMain::islogin(const ScanRet ret)
{
    if (ret == ScanRet::SUCCESS && (bool)userinfo["auto_exit"] == true)
    {
        exit(0);
    }
    pBtStop();
    SetWindowToFront();
    QMessageBox* messageBox = new QMessageBox(this);
    auto Show_QMessageBox = [&](const QString& title, const QString& text) {
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
    case ScanRet::STREAMERROR:
        Show_QMessageBox("提示", "直播流初始化失败!");
        break;
    case ScanRet::SUCCESS:
        Show_QMessageBox("提示", "扫码成功!");
        break;
    default:
        break;
    }
}

void WindowMain::loginConfirmTip(const GameType gameType, bool b)
{
    QString info("正在使用账号" + ui.lineEditUname->text());
    switch (gameType)
    {
    case GameType::Honkai3:
        info += "\n登录崩坏3\n";
        break;
    case GameType::Honkai3_BiliBili:
        info += "\n登录BiliBili崩坏3\n";
        break;
    case GameType::Genshin:
        info += "\n登录原神\n";
        break;
    case GameType::HonkaiStarRail:
        info += "\n登录星穹铁道\n";
        break;
    case GameType::ZenlessZoneZero:
        info += "\n登录绝区零\n";
        break;
    default:
        break;
    }
    SetWindowToFront();
    QMessageBox* messageBox = new QMessageBox(this);
    messageBox->setWindowTitle("登录确认");
    messageBox->setText(info + "确认登录？");
    QAbstractButton* yesButton = messageBox->addButton("登录", QMessageBox::YesRole);
    QAbstractButton* noButton = messageBox->addButton("取消", QMessageBox::NoRole);
    messageBox->exec();
    pBtStop();
    if (messageBox->clickedButton() != yesButton)
    {
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

void WindowMain::checkBoxAutoScreen(bool clicked)
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

void WindowMain::checkBoxAutoExit(bool clicked)
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

void WindowMain::checkBoxAutoLogin(bool clicked)
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

void WindowMain::liveIdError(const LiveStreamStatus status)
{
    switch (status)
    {
        using enum LiveStreamStatus;
    case Absent:
    {
        QMessageBox::information(this, "提示", "直播间不存在!", QMessageBox::Yes);
        return;
    }
    case NotLive:
    {
        QMessageBox::information(this, "提示", "直播间未开播！", QMessageBox::Yes);
        return;
    }
    case Error:
    {
        QMessageBox::information(this, "提示", "直播间未知错误!", QMessageBox::Yes);
        return;
    }
    default:
        return;
    }
    return;
}

int WindowMain::getSelectedRowIndex()
{
    QList<QTableWidgetItem*> item = ui.tableWidget->selectedItems();
    if (item.count() == 0)
    {
        return -1;
    }
    return ui.tableWidget->row(item.at(0));
}

bool WindowMain::checkDuplicates(const std::string uid)
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

bool WindowMain::GetStreamLink(const std::string& roomid, std::string& url, std::map<std::string, std::string>& heards)
{
    std::uint32_t live_type = ui.comboBox->currentIndex();
    switch (live_type)
    {
    case 0:
    {
        LiveBili live(roomid);
        if (LiveStreamStatus status = live.GetLiveStreamStatus(); status != LiveStreamStatus::Normal)
        {
            emit LiveStreamLinkError(status);
            return false;
        }
        heards = {
            { "user_agent", "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) \
            Chrome/110.0.0.0 Safari/537.36 Edg/110.0.1587.41" },
            { "referer", "https://live.bilibili.com" }
        };
        url = live.GetLiveStreamLink();
        break;
    }
    case 1:
    {
        LiveDouyin live(roomid);
        if (LiveStreamStatus status = live.GetLiveStreamStatus(); status != LiveStreamStatus::Normal)
        {
            emit LiveStreamLinkError(status);
            return false;
        }
        url = live.GetLiveStreamLink();
        break;
    }
    case 2:
    {
        LiveHuya live(roomid);
        if (LiveStreamStatus status = live.GetLiveStreamStatus(); status != LiveStreamStatus::Normal)
        {
            emit LiveStreamLinkError(status);
            return false;
        }
        heards = {
            { "user_agent", "Mozilla/5.0 (iPhone; CPU iPhone OS 16_6 like Mac OS X) AppleWebKit/605.1.15 (KHTML, like Gecko) Version/16.6 Mobile/15E148 Safari/604.1" }
        };
        url = live.GetLiveStreamLink();
        break;
    }
    default:
        break;
    }
    return true;
}

void WindowMain::SetWindowToFront() const
{
    HWND hwnd = reinterpret_cast<HWND>(winId());
    if (GetForegroundWindow() == hwnd)
        return;
    ShowWindow(hwnd, SW_MINIMIZE);
    ShowWindow(hwnd, SW_SHOWNORMAL);
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
    SetForegroundWindow(hwnd);
    SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
}

void WindowMain::failure()
{
    QMessageBox* messageBox = new QMessageBox(this);
    messageBox->setAttribute(Qt::WA_DeleteOnClose);
    messageBox->setText("登录状态失效，\n请重新添加账号！");
    messageBox->setWindowTitle("提示");
    messageBox->setIcon(QMessageBox::Information);
    messageBox->show();
}

void WindowMain::getInfo(int x, int y)
{
    QTableWidgetItem* item = ui.tableWidget->item(x, 2);
    QString cellText = item->text();
    ui.lineEditUname->setText(cellText);
    countA = x;

    trrlog::Log_debug("{}", std::format(R"(row = {} , user_name = {})", x, cellText.toStdString()));
}

void WindowMain::SetDefaultAccount()
{
    int nCurrentRow = getSelectedRowIndex();
    if (nCurrentRow != -1)
    {
        //ui.tableWidget->setCurrentCell(nCurrentRow, QItemSelectionModel::Current);
        userinfo["last_account"] = nCurrentRow + 1;
        m_config->updateConfig(userinfo.str());
        QMessageBox::information(this, "设置成功！", "勾选下方\"启动时自动监视屏幕\"\n将在下次启动时自动扫描并使用该账号登录", QMessageBox::Yes);
        return;
    }
    else
    {
        QMessageBox::information(this, "提示", "没有选择任何账号", QMessageBox::Yes);
        return;
    }
}

void WindowMain::DeleteAccount()
{
    int nCurrentRow = getSelectedRowIndex();
    if (nCurrentRow == -1)
    {
        QMessageBox::information(this, "提示", "没有选择任何账号", QMessageBox::Yes);
        return;
    }
    if (int re = QMessageBox::information(this, "删除确认", QString::fromStdString(std::format("你正在删除账号\n{}", (std::string)userinfo["account"][countA]["name"])), QMessageBox::Yes | QMessageBox::No);
        re != QMessageBox::Yes)
    {
        return;
    }
    userinfo["num"] = (int)userinfo["num"] - 1;
    //判断删除的账号是否为启动默认账号
    if (static_cast<int>(userinfo["last_account"]) == countA + 1)
    {
        userinfo["last_account"] = 0;
    }
    else if (static_cast<int>(userinfo["last_account"]) > countA + 1)
    {
        userinfo["last_account"] = static_cast<int>(userinfo["last_account"]) - 1;
    }
    userinfo["account"].remove(countA);

    trrlog::Log_debug("{}", userinfo.str());

    m_config->updateConfig(userinfo.str());
    //ui.tableWidget->setCurrentCell(nCurrentRow, QItemSelectionModel::Current);
    ui.tableWidget->removeRow(nCurrentRow);
    ui.tableWidget->clearSelection();
    ui.lineEditUname->setText("未选中");
    countA = -1;

    disconnect(ui.tableWidget, &QTableWidget::itemChanged, this, &WindowMain::updateNote);
    for (int i = 0; i < (int)userinfo["num"]; i++)
    {
        QTableWidgetItem* item = new QTableWidgetItem(QString("%1").arg(i + 1));
        ui.tableWidget->setItem(i, 0, item);
    }
    connect(ui.tableWidget, &QTableWidget::itemChanged, this, &WindowMain::updateNote);
}

void WindowMain::pBtStop()
{
    t1.stop();
    t2.stop();
    ui.pBtstartScreen->setText("监视屏幕");
    ui.pBtStream->setText("监视直播间");
    ui.pBtstartScreen->setChecked(false);
    ui.pBtStream->setChecked(false);
    ui.pBtstartScreen->setEnabled(true);
    ui.pBtStream->setEnabled(true);
}

void WindowMain::configInitUpdate(bool b)
{
    ui.tableWidget->blockSignals(true);
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
        ui.pBtstartScreen->clicked(true);
        ui.pBtstartScreen->setChecked(true);
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
    ui.tableWidget->blockSignals(false);
}

void WindowMain::updateNote(QTableWidgetItem* item)
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
            [[maybe_unused]] std::string s = data["account"][i]["uid"];
            s = data["account"][i]["name"];
            s = data["account"][i]["type"];
            s = data["account"][i]["note"];
            s = data["account"][i]["mid"];
        };
        [[maybe_unused]] int i = data["last_account"];
        [[maybe_unused]] bool b = data["auto_start"];
        b = data["auto_login"];
        b = data["auto_exit"];
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
