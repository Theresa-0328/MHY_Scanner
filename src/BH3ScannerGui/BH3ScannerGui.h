#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_BH3ScannerGui.h"
#include "LoginBili.h"
#include "ThreadGetScreen.h"
class BH3ScannerGui : public QMainWindow
{
    Q_OBJECT

public:
    BH3ScannerGui(QWidget *parent = nullptr);
    ~BH3ScannerGui();
    virtual void closeEvent(QCloseEvent* event);
    virtual void showEvent(QShowEvent* event);
    void islogin(const bool& b);
public slots:
    void pBtLoginAccount();
    void pBtstartScreen();
private:
    Ui::BH3ScannerGuiClass ui;
    ThreadGetScreen t1;
};
