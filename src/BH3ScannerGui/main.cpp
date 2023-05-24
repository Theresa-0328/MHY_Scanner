//https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1677936279&ticket=6437b1eafd72a209bb1e9ca5
#include "BH3ScannerGui.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>

bool isOpenMore();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (isOpenMore())
    {
        QMessageBox::warning(NULL, "错误", "程序正在运行");
        return -1;
    }
    BH3ScannerGui w;
    w.show();
    return a.exec();
}

bool isOpenMore() 
{
    HANDLE hMutex = CreateMutex(NULL, true, TEXT("T08lJ8CJmJiyoxdV"));
    return GetLastError() == ERROR_ALREADY_EXISTS;
}
