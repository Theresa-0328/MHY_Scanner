#include "LoginWindow.h"

#include <QRegularExpressionValidator>
#include <iostream>

LoginWindow::LoginWindow(QDialog* Dialog) :
    widget(new QWidget(this)),
    QRCodeImage(450, 450, QImage::Format_RGB32),
    label(new QLabel(widget))
{
    ui.setupUi(this);
    connect(ui.checkBoxShowPw, &QCheckBox::stateChanged, this, &LoginWindow::showPassword);
    connect(ui.pBtofficialLogin, &QPushButton::clicked, this, &LoginWindow::officialLogin);
    connect(ui.pBtofficialreject, &QPushButton::clicked, this, &LoginWindow::officialreject);
    connect(ui.tabWidget, &QTabWidget::currentChanged, this, &LoginWindow::controlQRCodeImage);

    ui.lineEditAccount->setPlaceholderText("请输入账号...");
    ui.lineEditAccount->setClearButtonEnabled(true);
    ui.lineEditAccount->setFont(QFont("宋体", 13));

    ui.lineEditPwd->setPlaceholderText("请输入密码...");
    ui.lineEditPwd->setClearButtonEnabled(true);
    ui.lineEditPwd->setFont(QFont("宋体", 13));
    ui.lineEditPwd->setEchoMode(QLineEdit::Password);

    ui.lineEditCookie->setFont(QFont("宋体", 13));
    ui.lineEditCookie->setPlaceholderText("在这里粘贴Cookie...");
    ui.lineEditCookie->setClearButtonEnabled(true);

    ui.tabWidget->setTabText(0, "官服Cookie");
    ui.tabWidget->insertTab(1, widget, "官服扫码");
    ui.tabWidget->setTabText(2, "崩坏3B服");
    ui.tabWidget->setCurrentIndex(0);
    ui.tabWidget->usesScrollButtons();

    QRCodeImage.fill(Qt::white);
    label->setPixmap(QPixmap::fromImage(QRCodeImage));
    //int currentIndex = ui.tabWidget->currentIndex();
}

void LoginWindow::officialLogin()
{
    QString c = ui.lineEditCookie->text();
    ClearInputBox();
    cookie = c.toStdString();
    QDialog::accept();
    type = 1;
}

void LoginWindow::officialreject()
{
    ClearInputBox();
    isReject = true;
    QDialog::reject();
}

void LoginWindow::controlQRCodeImage(int index)
{
    switch (index)
    {
    case 0:
        break;
    case 1:
    {
        //todo :占位图
        QPainter painter(&QRCodeImage);
        painter.setPen(Qt::NoPen);
        painter.setBrush(Qt::darkGreen);
        painter.drawRect(50, 50, 350, 350);
        painter.end();
        label->setPixmap(QPixmap::fromImage(QRCodeImage));
    }
    break;
    case 2:
        break;
    default:
        break;
    }
    std::cout << "controlQrcodeimage" << index << std::endl;
}

void LoginWindow::accept()
{
    QString a = ui.lineEditAccount->text();
    Account = a.toStdString();
    QString b = ui.lineEditPwd->text();
    ClearInputBox();
    Pwd = b.toStdString();
    QDialog::accept();
    isReject = false;
    type = 2;
}

void LoginWindow::reject()
{
    ClearInputBox();
    isReject = true;
    QDialog::reject();
}

void LoginWindow::ClearInputBox()
{
    ui.lineEditAccount->setText("");
    ui.lineEditPwd->setText("");
    ui.lineEditCookie->setText("");
}

void LoginWindow::getAccountPwd(std::string& Account, std::string& Pwd)
{
    Pwd = this->Pwd;
    Account = this->Account;
}

bool LoginWindow::getIsReject()
{
    return isReject;
}

void LoginWindow::showPassword(int state)
{
    if (state == Qt::Checked)
    {
        ui.lineEditPwd->setEchoMode(QLineEdit::Normal);
        ui.lineEditPwd->setInputMethodHints(Qt::ImhHiddenText);
    }
    else if (state == Qt::Unchecked)
    {
        ui.lineEditPwd->setEchoMode(QLineEdit::Password);
    }
}
