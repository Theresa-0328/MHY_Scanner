#include "ScannerGui.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>
#include <opencv2/core/utils/logger.hpp>

bool isOpen()
{
	HANDLE hMutex = CreateMutex(NULL, true, TEXT("T08lJ8CJmJiyoxdV"));
	return GetLastError() == ERROR_ALREADY_EXISTS;
}

int main(int argc, char* argv[])
{
	UINT utf8 = 65001;
	SetConsoleOutputCP(utf8);
	cv::utils::logging::setLogLevel(cv::utils::logging::LOG_LEVEL_ERROR);
	QApplication a(argc, argv);
	if (isOpen())
	{
		QMessageBox::warning(NULL, "错误", "程序正在运行");
		return -1;
	}
	ScannerGui w;
	w.show();
	return a.exec();
}
