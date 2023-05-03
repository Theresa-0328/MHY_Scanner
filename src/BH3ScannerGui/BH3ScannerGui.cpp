#include "BH3ScannerGui.h"
#include <QMessageBox>
#include <QWindow>

BH3ScannerGui::BH3ScannerGui(QWidget* parent)
	: QMainWindow(parent)
	, t1(this)
{
	ui.setupUi(this);
	ui.pBtLoginAccount->setStyleSheet(
		//正常状态样式
		"QPushButton{"
		"background-color:rgba(100,225,100,30);"//背景色（也可以设置图片）
		"border-style:outset;"                  //边框样式（inset/outset）
		"border-width:4px;"                     //边框宽度像素
		"border-radius:10px;"                   //边框圆角半径像素
		"border-color:rgba(255,255,255,30);"    //边框颜色
		"font:bold 13px;"                       //字体，字体大小
		"color:rgba(0,0,0,100);"                //字体颜色
		"padding:6px;"                          //填衬
		"}"
		//鼠标悬停样式
		"QPushButton:hover{"
		"background-color:rgba(100,255,100,100);"
		"border-color:rgba(255,255,255,200);"
		"color:rgba(0,0,0,200);"
		"}"
		//鼠标按下样式
		"QPushButton:pressed{"
		"background-color:rgba(100,255,100,200);"
		"border-color:rgba(255,255,255,30);"
		"border-style:inset;"
		"padding-left:10px;"
		"padding-top:10px;"
		"color:rgba(0,0,0,100);"
		"}");
	bool b1 = connect(ui.pBtLoginAccount, &QPushButton::clicked, this, &BH3ScannerGui::pBtLoginAccount);
	bool b2 = connect(ui.pBtstartScreen, &QPushButton::clicked, this, &BH3ScannerGui::pBtstartScreen);
	bool b3 = connect(&t1, &ThreadGetScreen::loginResults, this, &BH3ScannerGui::islogin);
	//ui.pBtstartScreen->clicked();
}

BH3ScannerGui::~BH3ScannerGui()
{
}

void BH3ScannerGui::pBtLoginAccount()
{

}

void BH3ScannerGui::pBtstartScreen()
{
	ui.pBtstartScreen->setText("监视屏幕二维码中");
	t1.start();
}

void BH3ScannerGui::closeEvent(QCloseEvent* event)
{

}

void BH3ScannerGui::showEvent(QShowEvent* event)
{

}

void BH3ScannerGui::islogin(const bool& b)
{
	if (b)
	{
		QMessageBox msgBox(QMessageBox::Information,
			"提示",
			"扫码登录成功！",
			QMessageBox::Yes,
			this);
		msgBox.exec();
	}
	else
	{

	}
}