#include "LoginWindow.h"
#include <QRegularExpressionValidator>

#define LOGINWINDOWREJECT 	ClearInputBox();isReject = true; QDialog::reject();

LoginWindow::LoginWindow(QDialog* Dialog)
{
	ui.setupUi(this);
	connect(ui.checkBoxShowPw, &QCheckBox::stateChanged, this, &LoginWindow::showPassword);
	connect(ui.pBtofficialLogin, &QPushButton::clicked, this, &LoginWindow::officialLogin);
	connect(ui.pBtofficialreject, &QPushButton::clicked, this, &LoginWindow::officialreject);
	ui.lineEditAccount->setPlaceholderText("请输入账号...");
	ui.lineEditAccount->setClearButtonEnabled(true);
	ui.lineEditAccount->setFont(QFont("宋体", 13));
	ui.lineEditPwd->setPlaceholderText("请输入密码...");
	ui.lineEditPwd->setClearButtonEnabled(true);
	ui.lineEditPwd->setFont(QFont("宋体", 13));
	ui.lineEditPwd->setEchoMode(QLineEdit::Password);
	ui.lineEditCookie->setFont(QFont("宋体", 13));
	ui.lineEditCookie->setPlaceholderText("在这里粘贴Cookie...");
	ui.tabWidget->setTabText(0, "官方");
	ui.tabWidget->setTabText(1, "崩坏3B服");
	ui.tabWidget->setCurrentIndex(0);
	ui.tabWidget->usesScrollButtons();
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
	LOGINWINDOWREJECT
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
	LOGINWINDOWREJECT
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
