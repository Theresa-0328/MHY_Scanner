#pragma once

#include <QThread>
#include <httplib.h>

class ThreadLocalServer : public QThread
{
	Q_OBJECT

public:
	ThreadLocalServer();
	~ThreadLocalServer();
	void run();
	void stop();
	std::string reCaptcha()const;
private:
	httplib::Server svr;
	std::string body;
};
