#include "LoginWindow.h"
#include <QRegularExpressionValidator>

LoginWindow::LoginWindow(QDialog* Dialog)
{
	ui.setupUi(this);
	ui.lineEditAccount->setPlaceholderText("请输入账号...");
	ui.lineEditAccount->setClearButtonEnabled(true);
	ui.lineEditAccount->setFont(QFont("宋体", 13));
	ui.lineEditPwd->setPlaceholderText("请输入密码...");
	ui.lineEditPwd->setClearButtonEnabled(true);
	ui.lineEditPwd->setFont(QFont("宋体", 13));
	ui.lineEditPwd->setEchoMode(QLineEdit::Password);
	ui.lineEditPwd->setInputMethodHints(Qt::ImhHiddenText);
	connect(ui.checkBoxShowPw, &QCheckBox::stateChanged, this, &LoginWindow::showPassword);
}

void LoginWindow::accept()
{
	QString a = ui.lineEditAccount->text();
	Account = a.toStdString();
	QString b = ui.lineEditPwd->text();
	Pwd = b.toStdString();
	QDialog::accept();
	isReject = false;
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
	}
	else if (state == Qt::Unchecked)
	{
		ui.lineEditPwd->setEchoMode(QLineEdit::Password);
	}
}
