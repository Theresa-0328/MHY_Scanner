#include "WindowLogin.h"

#include <thread>

#include <Windows.h>

#include <Base64.hpp>
#include <QThread>

#include "MhyApi.hpp"
#include "BH3Api.hpp"
#include "UtilMat.hpp"
#include "CookieParser.hpp"

WindowLogin::WindowLogin(QWidget* parent) :
    QWidget(parent),
    tabWidget(new QTabWidget(this))
{
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    setMinimumSize(QSize(500, 550));
    setMaximumSize(QSize(500, 550));
    setWindowTitle("添加账号");

    tabWidget->setMinimumSize(QSize(500, 550));
    tabWidget->setMaximumSize(QSize(500, 550));
    for (size_t i{}; i < tabs.size(); i++)
    {
        tabs[i] = new QWidget();
        tabWidget->addTab(tabs[i], tabsName[i].data());
        tabWidget->setFont(QFont("微软雅黑", 11));
    }
    InitTabs0();
    InitTabs1();
    InitTabs2();
    InitTabs3();
    MainHLayout = new QHBoxLayout(this);
    MainHLayout->setContentsMargins(0, 0, 0, 0);
    MainHLayout->addWidget(tabWidget);

    Initconnect();
}

WindowLogin::~WindowLogin()
{
    QrcodePool.clear();
}

void WindowLogin::closeEvent(QCloseEvent* event)
{
    deleteLater();
}

void WindowLogin::InitTabs0()
{
    Tab0MainVLayout = new QVBoxLayout(tabs[0]);
    Tab0MainVLayout->setSpacing(0);
    Tab0MainVLayout->setContentsMargins(15, 15, 15, 15);

    Tab0HLayout0 = new QHBoxLayout();
    Tab0HLayout0->setSpacing(10);
    Tab0HLayout0->setContentsMargins(30, 60, 30, 0);

    AreaCodeLabel = new QLabel(tabs[0]);
    AreaCodeLabel->setAlignment(Qt::AlignCenter);
    AreaCodeLabel->setFont(QFont("微软雅黑", 14));
    AreaCodeLabel->setText("+86");
    Tab0HLayout0->addWidget(AreaCodeLabel);

    phoneNumberLineEdit = new QLineEdit(tabs[0]);
    phoneNumberLineEdit->setMinimumSize(QSize(0, 50));
    phoneNumberLineEdit->setFont(QFont("微软雅黑", 14));
    QRegularExpression regex("^[1][3-9][0-9]{9}$");
    QRegularExpressionValidator* validator = new QRegularExpressionValidator(regex, this);
    phoneNumberLineEdit->setValidator(validator);
    phoneNumberLineEdit->setPlaceholderText("请输入手机号码");
    phoneNumberLineEdit->setClearButtonEnabled(true);
    Tab0HLayout0->addWidget(phoneNumberLineEdit);

    pbtSend = new QPushButton(tabs[0]);
    QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(pbtSend->sizePolicy().hasHeightForWidth());
    pbtSend->setSizePolicy(sizePolicy);
    pbtSend->setMinimumSize(QSize(50, 50));
    pbtSend->setFont(QFont("微软雅黑", 14));
    pbtSend->setText("发送");
    Tab0HLayout0->addWidget(pbtSend);

    Tab0MainVLayout->addLayout(Tab0HLayout0);

    Tab0HLayout1 = new QHBoxLayout();
    Tab0HLayout1->setSpacing(0);
    Tab0HLayout1->setContentsMargins(30, -1, 30, 30);
    verifyCodeLineEdit = new QLineEdit(tabs[0]);
    verifyCodeLineEdit->setMinimumSize(QSize(0, 50));
    verifyCodeLineEdit->setFont(QFont("微软雅黑", 14));
    verifyCodeLineEdit->setPlaceholderText("请输入收到的验证码");
    verifyCodeLineEdit->setClearButtonEnabled(true);
    Tab0HLayout1->addWidget(verifyCodeLineEdit);

    Tab0MainVLayout->addLayout(Tab0HLayout1);

    Tab0HLayout2 = new QHBoxLayout();
    Tab0HLayout2->setSpacing(50);
    Tab0HLayout2->setContentsMargins(30, -1, 30, 80);
    Tab0pbtConfirm = new QPushButton(tabs[0]);
    Tab0pbtConfirm->setMinimumSize(QSize(0, 50));
    Tab0pbtConfirm->setFont(QFont("微软雅黑", 14));
    Tab0pbtConfirm->setText("确认");
    Tab0HLayout2->addWidget(Tab0pbtConfirm);
    Tab0pbtCancel = new QPushButton(tabs[0]);
    Tab0pbtCancel->setFont(QFont("微软雅黑", 14));
    Tab0pbtCancel->setText("取消");
    Tab0pbtCancel->setMinimumSize(QSize(0, 50));
    Tab0HLayout2->addWidget(Tab0pbtCancel);

    Tab0MainVLayout->addLayout(Tab0HLayout2);

    Tabs0Timer = new QTimer(this);

    pbtSend->setEnabled(false);
    Tab0pbtConfirm->setEnabled(false);
}

void WindowLogin::InitTabs1()
{
    Tab1MainVLayout = new QVBoxLayout(tabs[1]);

    Prompt = new QLabel(tabs[1]);
    Prompt->setText("打开米游社APP，扫一扫登录");
    Prompt->setMaximumSize(QSize(100000, 50));
    Prompt->setFont(QFont("微软雅黑", 15));
    Prompt->setAlignment(Qt::AlignCenter);

    Tab1MainVLayout->addWidget(Prompt);

    QRCodelabel = new QLabel(tabs[1]);

    Tab1MainVLayout->addWidget(QRCodelabel);

    QRCodeQImage.fill(255);
    QRCodelabel->setPixmap(QPixmap::fromImage(QRCodeQImage));
    QRCodelabel->setAlignment(Qt::AlignCenter);
    QRCodelabel->setText("二维码加载中");
    QRCodelabel->setFont(QFont("微软雅黑", 15));
    //QRCodelabel->setStyleSheet("border: 2px solid black;");

    UpdateQrcodeButton = new QPushButton("二维码已过期\n点击刷新二维码", tabs[1]);
    UpdateQrcodeButton->setFont(QFont("微软雅黑", 13));
    UpdateQrcodeButton->setFixedSize(170, 170);
    UpdateQrcodeButton->setVisible(false);

    QrcodeTimer = new QTimer(this);

    QrcodePool.setMaxThreadCount(1);
}

void WindowLogin::InitTabs2()
{
    Tab2MainVLayout = new QVBoxLayout(tabs[2]);
    Tab2MainVLayout->setSpacing(0);
    Tab2MainVLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    Tab2MainVLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    Tab2MainVLayout->setContentsMargins(30, 80, 30, 0);

    lineEditCookie = new QLineEdit(tabs[2]);
    lineEditCookie->setObjectName(QString::fromUtf8("lineEditCookie"));
    lineEditCookie->setMinimumSize(QSize(0, 50));
    lineEditCookie->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    lineEditCookie->setFont(QFont("微软雅黑", 14));
    lineEditCookie->setPlaceholderText("在这里粘贴包含SToken的Cookie...");
    lineEditCookie->setClearButtonEnabled(true);
    Tab2MainVLayout->addWidget(lineEditCookie);

    pBtofficialLogin = new QPushButton(tabs[2]);
    pBtofficialLogin->setMinimumSize(QSize(0, 50));
    pBtofficialLogin->setText("登录");
    pBtofficialLogin->setFont(QFont("微软雅黑", 13));
    pBtofficialreject = new QPushButton(tabs[2]);
    pBtofficialreject->setMinimumSize(QSize(0, 50));
    pBtofficialreject->setText("取消");
    pBtofficialreject->setFont(QFont("微软雅黑", 13));

    Tab2ButtonHLayout = new QHBoxLayout();
    Tab2ButtonHLayout->addWidget(pBtofficialLogin);
    Tab2ButtonHLayout->addWidget(pBtofficialreject);
    Tab2ButtonHLayout->setContentsMargins(10, 50, 10, 60);
    Tab2ButtonHLayout->setSpacing(50);
    Tab2MainVLayout->addLayout(Tab2ButtonHLayout);
}

void WindowLogin::InitTabs3()
{
    Tab3MainVLayout = new QVBoxLayout(tabs[3]);
    Tab3MainVLayout->setStretch(0, 3);
    Tab3MainVLayout->setStretch(2, 2);

    Tab3VLayout0 = new QVBoxLayout();
    Tab3VLayout0->setSpacing(40);
    Tab3VLayout0->setContentsMargins(50, 20, 50, -1);
    lineEditAccount = new QLineEdit(tabs[3]);
    lineEditAccount->setMaximumSize(QSize(400, 40));
    lineEditAccount->setPlaceholderText("请输入账号...");
    lineEditAccount->setClearButtonEnabled(true);
    Tab3VLayout0->addWidget(lineEditAccount);
    lineEditPwd = new QLineEdit(tabs[3]);
    lineEditPwd->setMaximumSize(QSize(400, 40));
    lineEditPwd->setPlaceholderText("请输入密码...");
    lineEditPwd->setClearButtonEnabled(true);
    lineEditPwd->setEchoMode(QLineEdit::Password);
    Tab3VLayout0->addWidget(lineEditPwd);

    Tab3MainVLayout->addLayout(Tab3VLayout0);

    Tab3VLayout1 = new QVBoxLayout();
    Tab3VLayout1->setContentsMargins(300, -1, -1, -1);
    checkBoxShowPw = new QCheckBox(tabs[3]);
    checkBoxShowPw->setText("显示密码");
    Tab3VLayout1->addWidget(checkBoxShowPw);

    Tab3MainVLayout->addLayout(Tab3VLayout1);

    Tab3HLayout2 = new QHBoxLayout();
    Tab3HLayout2->setSpacing(40);
    Tab3HLayout2->setContentsMargins(50, 30, 50, 125);
    Tab3pBtConfirm = new QPushButton(tabs[3]);
    Tab3pBtConfirm->setText("登录");
    Tab3pBtConfirm->setMinimumSize(120, 35);
    Tab3HLayout2->addWidget(Tab3pBtConfirm);
    Tab3pBtCancel = new QPushButton(tabs[3]);
    Tab3pBtCancel->setText("取消");
    Tab3pBtCancel->setMinimumSize(120, 35);
    Tab3HLayout2->addWidget(Tab3pBtCancel);

    Tab3MainVLayout->addLayout(Tab3HLayout2);
}

void WindowLogin::Initconnect()
{
    connect(this, &WindowLogin::showMessagebox, this, [this](const QString& Message) {
        QMessageBox::information(this, "提示", Message, QMessageBox::Yes);
    });

    connect(this, &WindowLogin::showWindowGeeTest, this, [this](const bool show) {
        if (show)
        {
            m_WindowGeeTest.show();
        }
        else
        {
            m_WindowGeeTest.close();
        }
    });

    connect(this, &WindowLogin::QrcodeLoginResult, this, [this](bool result) {
        if (result)
        {
            QrcodeTimer->stop();
        }
        else
        {
            QRCodeQImage = CV_8UC1_MatToQImage(QrcodeMat - cv::Scalar(200));
            QRCodelabel->setPixmap(QPixmap::fromImage(QRCodeQImage));
            UpdateQrcodeButton->setVisible(true);
        }
    });

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index) {
        QrcodeTimer->stop();
        QRCodeQImage.fill(255);
        QRCodelabel->setText("二维码加载中");
        switch (index)
        {
        case 0:
        {
            break;
        }
        case 1:
        {
            StartQRCodeLogin();
            break;
        }
        case 2:
        {
            break;
        }
        case 3:
        {
            break;
        }
        default:
            __assume(0);
        }
    });

    connect(phoneNumberLineEdit, &QLineEdit::textChanged, this, [this] {
        if (phoneNumberLineEdit->text().toStdString().size() >= 11)
        {
            pbtSend->setEnabled(true);
        }
        else
        {
            pbtSend->setEnabled(false);
        }
    });

    connect(&m_WindowGeeTest, &WindowGeeTest::postMessage, this, [this](const QString& Message) {
        m_WindowGeeTest.close();
        if (GeeTestInfo.GeeTestType == GeeTestInfo.BiLi)
        {
            QThreadPool::globalInstance()->start([this, Message]() {
                QJsonObject json{ QJsonDocument{ QJsonDocument::fromJson(Message.toUtf8()) }.object() };
                auto result{ BH3API::BILI::LoginByPassWord(
                    lineEditAccount->text().toStdString(), lineEditPwd->text().toStdString(),
                    GeeTestInfo.gt_user_id,
                    json.value("geetest_challenge").toString().toStdString(),
                    json.value("geetest_validate").toString().toStdString()) };
                ResultByLoginBH3BiLiBiLi(result);
            });
        }
        else if (GeeTestInfo.GeeTestType == GeeTestInfo.Official)
        {
            QThreadPool::globalInstance()->start([this, Message]() {
                GeeTestInfo.Aigis = GeeTestInfo.GeetestSessionId + ";" + Message.toUtf8().toBase64().toStdString();
                auto result = CreateLoginCaptcha(GeeTestInfo.phoneNumber, GeeTestInfo.Aigis);
                if (result.retcode == 0)
                {
                    GeeTestInfo.action_type = result.action_type;
                    emit ButtonEnabled(true);
                }
                else if (result.retcode == -3006)
                {
                    emit showMessagebox("请求过于频繁，请稍后再试");
                }
            });
        }
    });

    connect(Tabs0Timer, &QTimer::timeout, this, [this]() {
        remainingSeconds--;
        pbtSend->setText(QString::number(remainingSeconds) + "秒");
        if (remainingSeconds <= 0)
        {
            Tabs0Timer->stop();
            pbtSend->setEnabled(true);
            pbtSend->setText("发送");
        }
    });

    connect(this, &WindowLogin::ButtonEnabled, this, [this](bool enabled) {
        if (enabled)
        {
            pbtSend->setEnabled(false);
            Tabs0Timer->start(1000);
            remainingSeconds = 60;
        }
        Tab0pbtConfirm->setEnabled(enabled);
    });

    connect(pbtSend, &QPushButton::clicked, this, [this] {
        QThreadPool::globalInstance()->start([this] {
            GeeTestInfo.phoneNumber = phoneNumberLineEdit->text().toStdString();
            auto result{ CreateLoginCaptcha(GeeTestInfo.phoneNumber) };
            if (result.mmt_type)
            {
                GeeTestInfo.GeetestSessionId = result.session_id;
                GeeTestInfo.GeeTestType = GeeTestInfo.Official;
                m_WindowGeeTest.Init(stringTowstring(result.gt), stringTowstring(result.challenge));
                emit showWindowGeeTest(true);
            }
            else if (!result.mmt_type)
            {
                emit ButtonEnabled(true);
            }
            else if (result.retcode == -3008)
            {
                emit showMessagebox("手机号错误");
            }
        });
    });

    connect(Tab0pbtConfirm, &QPushButton::clicked, this, [this] {
        QThreadPool::globalInstance()->start([this] {
            auto result = LoginByMobileCaptcha(GeeTestInfo.action_type, GeeTestInfo.phoneNumber, verifyCodeLineEdit->text().toStdString());
            if (result.retcode == -3205)
            {
                emit showMessagebox("短信验证码错误");
            }
            else if (result.retcode == 0)
            {
                const std::string name{ getMysUserName(result.data.aid) };
                emit AddUserInfo(name, result.data.V2Token, result.data.aid, result.data.mid, "官服");
            }
        });
    });

    connect(Tab3pBtConfirm, &QPushButton::clicked, this, [this] {
        QThreadPool::globalInstance()->start([this] {
            auto result{ BH3API::BILI::LoginByPassWord(lineEditAccount->text().toStdString(), lineEditPwd->text().toStdString()) };
            ResultByLoginBH3BiLiBiLi(result);
        });
    });

    connect(pBtofficialLogin, &QPushButton::clicked, this, [this] {
        QThreadPool::globalInstance()->start([this] {
            std::string CookieString{ lineEditCookie->text().toStdString() };
            CookieParser cp(CookieString);
            std::string uid{}, stoken{}, mid{};
            if (auto value = cp.GetCookieValue("stuid"); value.has_value())
            {
                uid = *value;
            }
            else if (auto value = cp.GetCookieValue("ltuid"); value.has_value())
            {
                uid = *value;
            }
            else if (auto value = cp.GetCookieValue("account_id"); value.has_value())
            {
                uid = *value;
            }
            else
            {
                emit showMessagebox("Cookie格式错误!");
                return;
            }
            if (auto value = cp.GetCookieValue("stoken"); value.has_value())
            {
                stoken = *value;
            }
            else
            {
                emit showMessagebox("Cookie格式错误!");
                return;
            }
            if (auto value = cp.GetCookieValue("mid"); value.has_value())
            {
                mid = *value;
            }
            else
            {
                emit showMessagebox("Cookie格式错误!");
                return;
            }
            const std::string name{ getMysUserName(uid) };
            emit AddUserInfo(name, stoken, uid, mid, "官服");
        });
    });

    connect(UpdateQrcodeButton, &QPushButton::clicked, this, &WindowLogin::StartQRCodeLogin);

    connect(QrcodeTimer, &QTimer::timeout, this, [this]() {
        QThreadPool::globalInstance()->start([this]() {
            CheckQRCodeLoginState();
        });
    });

    connect(this, &WindowLogin::StartQRCodeTimer, this, [this]() {
        if (!QrcodeTimer->isActive())
        {
            QrcodeTimer->start(1000);
        }
    });

    connect(checkBoxShowPw, &QCheckBox::checkStateChanged, this, [this](int state) {
        if (state == Qt::Checked)
        {
            lineEditPwd->setEchoMode(QLineEdit::Normal);
            lineEditPwd->setInputMethodHints(Qt::ImhHiddenText);
        }
        else if (state == Qt::Unchecked)
        {
            lineEditPwd->setEchoMode(QLineEdit::Password);
        }
    });

    connect(Tab0pbtCancel, &QPushButton::clicked, this, [this]() {
        phoneNumberLineEdit->clear();
        verifyCodeLineEdit->clear();
    });

    connect(pBtofficialreject, &QPushButton::clicked, this, [this]() {
        lineEditCookie->clear();
    });

    connect(Tab3pBtCancel, &QPushButton::clicked, this, [this]() {
        lineEditAccount->clear();
        lineEditPwd->clear();
    });
}

void WindowLogin::ResultByLoginBH3BiLiBiLi(const auto& result)
{
    if (result.code == 0)
    {
        emit AddUserInfo(result.uname, result.access_key, result.uid, "", "崩坏3B服");
    }
    else if (result.code == 200000)
    {
        auto result = BH3API::BILI::CaptchaCaptcha();
        GeeTestInfo.gt_user_id = result.gt_user_id;
        GeeTestInfo.GeeTestType = GeeTestInfo.BiLi;
        m_WindowGeeTest.Init(stringTowstring(result.gt), stringTowstring(result.challenge));
        emit showWindowGeeTest(true);
    }
    else if (result.code == 500002)
    {
        emit showMessagebox("密码错误");
    }
    else if (result.code != 0)
    {
        emit showMessagebox(QString::fromStdString(result.message));
    }
}

void WindowLogin::ResultByLoginOfficial(const auto& result)
{
}

void WindowLogin::StartQRCodeLogin()
{
    QrcodeTimer->stop();
    UpdateQrcodeButton->hide();
    UpdateQrcodeButton->move(QRCodelabel->x() + (QRCodelabel->width() - 170) / 2, QRCodelabel->y() + (QRCodelabel->height() - 170) / 2);
    AllowDrawQRCode.store(true);
    QrcodePool.start([this]() {
        QrcodePool.clear();
        QRCodeQImage.fill(255);
        QRCodelabel->setText("二维码加载中");
        AllowDrawQRCode.store(false);
        const std::string qrcodeString{ GetLoginQrcodeUrl() };
        ticket = std::string{ qrcodeString.data() + qrcodeString.size() - 24, 24 };
        QrcodeMat = createQrCodeToCvMat(qrcodeString);
        QRCodeQImage = CV_8UC1_MatToQImage(QrcodeMat);
        if (AllowDrawQRCode.load())
        {
            return;
        }
        QRCodelabel->setPixmap(QPixmap::fromImage(QRCodeQImage));
        emit StartQRCodeTimer();
    });
}

void WindowLogin::CheckQRCodeLoginState()
{
    switch (auto result{ GetQRCodeState(ticket) }; result.StateType)
    {
    case result.Init:
    {
    }
    break;
    case result.Scanned:
    {
        QRCodelabel->setText("正在登录\n\n请在手机上点击「确认登录」");
    }
    break;
    case result.Confirmed:
    {
        if (const auto resultStoken{ getStokenByGameToken(result.uid, result.token) }; resultStoken.has_value())
        {
            std::string name{ getMysUserName(result.uid) };
            const auto& [mid, stoken] = *resultStoken;
            emit AddUserInfo(name, stoken, result.uid, mid, "官服");
            QRCodelabel->setText("登录成功！");
            emit QrcodeLoginResult(true);
        }
        else
        {
            emit showMessagebox("获取STOKEN失败！");
        }
        return;
    }
    break;
    case result.Expired:
    {
        emit QrcodeLoginResult(false);
        return;
    }
    break;
    default:
        __assume(0);
    }
}