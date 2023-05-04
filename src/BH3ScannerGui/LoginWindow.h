#pragma once

#include <QDialog>
#include "ui_LoginWindow.h"

class LoginWindow : public QDialog
{
	Q_OBJECT
public:
	LoginWindow(QDialog* Dialog = nullptr);
public slots:
	virtual void accept();
	virtual void reject();
	void ClearInputBox();
	void getAccountPwd(std::string& Pwd, std::string& Account);
	bool getIsReject();
private:
	Ui::Dialog ui;
	std::string Pwd;
	std::string Account;
	bool isReject = false;
};
