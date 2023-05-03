#pragma once

#include <QThread>
#include <QMutex>
#include "LoginBili.h"
class ThreadGetScreen  : public QThread, public LoginBili
{
	Q_OBJECT
public:
	ThreadGetScreen(QObject* parent);
	~ThreadGetScreen();
	bool isExit = true;
	void run();

signals:
	void loginResults(const bool& b);
private:
	QMutex m_mux;
	bool signed_in = true;
};