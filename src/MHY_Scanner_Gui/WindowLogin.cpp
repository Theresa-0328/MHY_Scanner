#include "WindowLogin.h"

#include <QThread>

WindowLogin::WindowLogin(QWidget* parent) :
    QWidget(parent),
    tabWidget(new QTabWidget(this))
{
    setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    setMinimumSize(QSize(450, 450));
    setMaximumSize(QSize(450, 450));
    tabWidget->setMinimumSize(QSize(450, 450));
    tabWidget->setMaximumSize(QSize(450, 450));
    for (size_t i{}; i < tabs.size(); i++)
    {
        tabs[i] = new QWidget();
        tabWidget->addTab(tabs[i], tabsName[i]);
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
}

void WindowLogin::InitTabs0()
{
    Tab0MainHLayout = new QHBoxLayout(tabs[0]);

    Tab0VLayout = new QVBoxLayout();
    Tab0VLayout->setSpacing(0);
    Tab0VLayout->setObjectName(QString::fromUtf8("verticalLayout"));
    Tab0VLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    Tab0VLayout->setContentsMargins(0, 80, 0, 0);

    lineEditCookie = new QLineEdit(tabs[0]);
    lineEditCookie->setObjectName(QString::fromUtf8("lineEditCookie"));
    lineEditCookie->setMinimumSize(QSize(0, 50));
    lineEditCookie->setAlignment(Qt::AlignLeading | Qt::AlignLeft | Qt::AlignVCenter);
    lineEditCookie->setFont(QFont("宋体", 14));
    lineEditCookie->setPlaceholderText("在这里粘贴包含SToken的Cookie...");
    lineEditCookie->setClearButtonEnabled(true);
    Tab0VLayout->addWidget(lineEditCookie);

    pBtofficialLogin = new QPushButton(tabs[0]);
    pBtofficialLogin->setMinimumSize(QSize(0, 50));
    pBtofficialLogin->setText("登录");
    pBtofficialLogin->setFont(QFont("宋体", 13));
    pBtofficialreject = new QPushButton(tabs[0]);
    pBtofficialreject->setMinimumSize(QSize(0, 50));
    pBtofficialreject->setText("取消");
    pBtofficialreject->setFont(QFont("宋体", 13));

    Tab0ButtonHLayout = new QHBoxLayout();
    Tab0ButtonHLayout->addWidget(pBtofficialLogin);
    Tab0ButtonHLayout->addWidget(pBtofficialreject);
    Tab0ButtonHLayout->setContentsMargins(10, 50, 10, 60);
    Tab0ButtonHLayout->setSpacing(50);
    Tab0VLayout->addLayout(Tab0ButtonHLayout);

    Tab0MainHLayout->addLayout(Tab0VLayout);
}

void WindowLogin::InitTabs1()
{
}

void WindowLogin::InitTabs2()
{
}

void WindowLogin::InitTabs3()
{
}

void WindowLogin::Initconnect()
{
    connect(pBtofficialLogin, &QPushButton::clicked, this, [this]() {
        emit signals_test1();
    });
}