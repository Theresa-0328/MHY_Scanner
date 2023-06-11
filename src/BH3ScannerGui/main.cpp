//https://user.mihoyo.com/qr_code_in_game.html?app_id=1&app_name=%E5%B4%A9%E5%9D%8F3&bbs=true&biz_key=bh3_cn&expire=1677936279&ticket=6437b1eafd72a209bb1e9ca5
//https://user.mihoyo.com/qr_code_in_game.html?app_id=8&app_name=%E5%B4%A9%E5%9D%8F%3A%E6%98%9F%E7%A9%B9%E9%93%81%E9%81%93&bbs=true&biz_key=hkrpg_cn&expire=1686822569&ticket=6484472959a1066e0648ae67
#include "BH3ScannerGui.h"
#include <QtWidgets/QApplication>
#include <QMessageBox>

bool isOpen()
{
    HANDLE hMutex = CreateMutex(NULL, true, TEXT("T08lJ8CJmJiyoxdV"));
    return GetLastError() == ERROR_ALREADY_EXISTS;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    if (isOpen())
    {
        QMessageBox::warning(NULL, "错误", "程序正在运行");
        return -1;
    }
    BH3ScannerGui w;
    w.show();
    return a.exec();
}
