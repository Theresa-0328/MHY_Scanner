#pragma once

#include <QDialog>
#include "ui_LoginWindow.h"

class LoginWindow : public QDialog
{
	Q_OBJECT
public:
	LoginWindow(QDialog* Dialog = nullptr);
	void getAccountPwd(std::string& Pwd, std::string& Account);
	std::string cookie;
	bool getIsReject();
	void ClearInputBox();
public slots:
	void showPassword(int state);
	virtual void accept();
	virtual void reject();
private:
	Ui::Dialog ui;
	std::string Pwd;
	std::string Account;
	bool isReject = false;
};
