#include "WindowLogin.h"

#include <Windows.h>

#include <QThread>
#include <Base64.hpp>

#include "MhyApi.hpp"

WindowLogin::WindowLogin(QWidget* parent) :
    QWidget(parent),
    tabWidget(new QTabWidget(this))
{
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    setMinimumSize(QSize(500, 550));
    setMaximumSize(QSize(500, 550));
    tabWidget->setMinimumSize(QSize(500, 550));
    tabWidget->setMaximumSize(QSize(500, 550));
    for (size_t i{}; i < tabs.size(); i++)
    {
        tabs[i] = new QWidget();
        tabWidget->addTab(tabs[i], tabsName[i]);
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

WindowLogin::~WindowLogin() = default;

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
}

void WindowLogin::InitTabs1()
{
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
    Tab3VLayout0->setSpacing(100);
    Tab3VLayout0->setContentsMargins(50, 20, 50, 50);
    lineEditAccount = new QLineEdit(tabs[3]);
    Tab3VLayout0->addWidget(lineEditAccount);
    lineEditPwd = new QLineEdit(tabs[3]);
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
    Tab3HLayout2->setContentsMargins(50, 30, 50, 100);
    pBt1 = new QPushButton(tabs[3]);
    pBt1->setText("登录");
    Tab3HLayout2->addWidget(pBt1);
    pBt2 = new QPushButton(tabs[3]);
    pBt2->setText("取消");
    Tab3HLayout2->addWidget(pBt2);

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

    connect(&m_WindowGeeTest, &WindowGeeTest::postMessage, this, [this](const QString& Message) {
        m_WindowGeeTest.close();
        GeeTestInfo.Aigis = GeeTestInfo.GeetestSessionId + ";" + Message.toUtf8().toBase64().toStdString();
        thpool.start([this, Message]() {
            auto result = CreateLoginCaptcha(GeeTestInfo.phoneNumber, GeeTestInfo.Aigis);
            if (result.retcode == 0)
            {
                GeeTestInfo.action_type = result.action_type;
            }
            else if (result.retcode == -3006)
            {
                emit showMessagebox("请求过于频繁，请稍后再试");
            }
        });
    });

    connect(pbtSend, &QPushButton::clicked, this, [this] {
        pbtSend->setEnabled(false);
        thpool.start([this] {
            GeeTestInfo.phoneNumber = phoneNumberLineEdit->text().toStdString();
            auto result{ CreateLoginCaptcha(GeeTestInfo.phoneNumber) };
            if (result.mmt_type)
            {
                GeeTestInfo.GeetestSessionId = result.session_id;
                m_WindowGeeTest.Init(stringTowstring(result.gt), stringTowstring(result.challenge));
                emit showWindowGeeTest(true);
            }
            //TODO 无需进行人机验证
            else
            {
            }
        });
    });

    connect(Tab0pbtConfirm, &QPushButton::clicked, this, [this] {
        thpool.start([this] {
            auto result = LoginByMobileCaptcha(GeeTestInfo.action_type, GeeTestInfo.phoneNumber, verifyCodeLineEdit->text().toStdString());
            if (result.retcode == -3205)
            {
                emit showMessagebox("验证码错误");
            }
            else if (result.retcode == 0)
            {
                const std::string name{ getMysUserName(result.data.aid) };
                emit AddUserInfo(name, result.data.V2Token, result.data.aid, result.data.mid, "官服");
            }
        });
    });
}