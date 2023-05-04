#pragma once

#include <QThread>
#include <QMutex>
#include "LoginBili.h"
class ThreadGetScreen
	: public QThread
{

	Q_OBJECT
public:
	ThreadGetScreen(QObject* parent);
	~ThreadGetScreen();
	bool isExit = true;
	void biliInit(int uid, std::string access_key, std::string uname);
	void run();

signals:
	void loginResults(const bool& b);
private:
	QMutex m_mux;
	bool signed_in = true;
	std::string LoginData;
	Mihoyosdk m;
};