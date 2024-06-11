#include "LoginWindow.h"

#include <Windows.h>

#include <QRegularExpressionValidator>

#include "MhyApi.hpp"
#include "UtilString.hpp"

LoginWindow::LoginWindow(QDialog* Dialog) :
    QRCodewidget(new QWidget(Dialog)),
    QRCodeImage(305, 305, QImage::Format_Indexed8),
    UpdateQrcodeButton(new QPushButton("二维码已过期\n点击刷新二维码")),
    QRCodelabel(new QLabel(QRCodewidget)),
    showQRcodeImage(true)
{
    ui.setupUi(this);
    connect(ui.checkBoxShowPw, &QCheckBox::stateChanged, this, &LoginWindow::showPassword);
    connect(ui.pBtofficialLogin, &QPushButton::clicked, this, &LoginWindow::officialLogin);
    connect(ui.pBtofficialreject, &QPushButton::clicked, this, &LoginWindow::officialreject);
    connect(ui.tabWidget, &QTabWidget::currentChanged, this, &LoginWindow::controlQRCodeImage);
    connect(UpdateQrcodeButton, &QPushButton::clicked, this, &LoginWindow::hideQrcodeButtonFun);
    connect(this, &LoginWindow::sendQrcodeButtonVisible, this, &LoginWindow::showQrcodeButtonFun);

    ui.lineEditAccount->setPlaceholderText("请输入账号...");
    ui.lineEditAccount->setClearButtonEnabled(true);
    ui.lineEditAccount->setFont(QFont("宋体", 13));

    ui.lineEditPwd->setPlaceholderText("请输入密码...");
    ui.lineEditPwd->setClearButtonEnabled(true);
    ui.lineEditPwd->setFont(QFont("宋体", 13));
    ui.lineEditPwd->setEchoMode(QLineEdit::Password);

    ui.lineEditCookie->setFont(QFont("宋体", 13));
    ui.lineEditCookie->setPlaceholderText("在这里粘贴Cookie...");
    ui.lineEditCookie->setClearButtonEnabled(true);

    ui.tabWidget->setTabText(0, "官服Cookie");
    ui.tabWidget->insertTab(1, QRCodewidget, "官服扫码");
    ui.tabWidget->setTabText(2, "崩坏3B服");
    ui.tabWidget->setCurrentIndex(0);
    ui.tabWidget->usesScrollButtons();

    QRCodeImage.fill(Qt::white);
    QRCodelabel->setPixmap(QPixmap::fromImage(QRCodeImage));
    QRCodelabel->setAlignment(Qt::AlignCenter);
    QRCodelabel->setFixedSize(QSize(305, 305));
    int labelWidth = QRCodelabel->width();
    int labelHeight = QRCodelabel->height();
    QRCodelabel->move((ui.tabWidget->width() - labelWidth) / 2, (ui.tabWidget->height() - labelHeight) / 2);
    QFont font;
    font.setPointSize(13);
    UpdateQrcodeButton->setFont(font);
    UpdateQrcodeButton->setParent(QRCodelabel);
    UpdateQrcodeButton->setFixedSize(170, 170);
    UpdateQrcodeButton->move((labelWidth - 170) / 2, (labelHeight - 170) / 2);
    UpdateQrcodeButton->setVisible(false);

    QRCodelabel->setFont(font);

    threadPool.setMaxThreadCount(1);
    //int currentIndex = ui.tabWidget->currentIndex();

    ui.pBtofficialLogin->setEnabled(false);
}

LoginWindow::~LoginWindow()
{
    threadPool.clear();
    threadPool.waitForDone();
}

void LoginWindow::officialLogin()
{
    QString c = ui.lineEditCookie->text();
    ClearInputBox();
    cookie = c.toStdString();
    QDialog::accept();
    type = 1;
}

void LoginWindow::officialreject()
{
    ClearInputBox();
    isReject = true;
    QDialog::reject();
}

void LoginWindow::controlQRCodeImage(int index)
{
    UpdateQrcodeButton->setVisible(false);
    showQRcodeImage = false;
    switch (index)
    {
    case 0:
    {
    }
    break;
    case 1:
    {
        UpdateQrcodeButton->hide();
        QRCodeImage.fill(255);
        QRCodelabel->setPixmap(QPixmap::fromImage(QRCodeImage));
        threadPool.clear();
        threadPool.start([&]() {showQRcodeImage = true; LoginWindow::qrcodeThreadFun(index); });
    }
    break;
    case 2:
    {
    }
    break;
    default:
        __assume(0);
    }
}

void LoginWindow::closeEvent(QCloseEvent* event)
{
    showQRcodeImage = false;
}

void LoginWindow::accept()
{
    QString a = ui.lineEditAccount->text();
    Account = a.toStdString();
    QString b = ui.lineEditPwd->text();
    ClearInputBox();
    Pwd = b.toStdString();
    QDialog::accept();
    isReject = false;
    type = 2;
}

void LoginWindow::reject()
{
    ClearInputBox();
    isReject = true;
    QDialog::reject();
}

void LoginWindow::ClearInputBox()
{
    ui.lineEditAccount->setText("");
    ui.lineEditPwd->setText("");
    ui.lineEditCookie->setText("");
}

void LoginWindow::qrcodeThreadFun(int index)
{
    const std::string deviceID{ createUUID4() };
    std::string qrcodeString{ GetLoginQrcodeUrl(deviceID) };
    replace0026WithAmpersand(qrcodeString);
    const std::string_view ticket{ qrcodeString.data() + qrcodeString.size() - 24, 24 };
    std::string accountData;
    QrcodeMat = createQrCodeToCvMat(qrcodeString);
    QRCodeImage = CV_8UC1_MatToQImage(QrcodeMat);
    QRCodelabel->setPixmap(QPixmap::fromImage(QRCodeImage));
    while (showQRcodeImage)
    {
        QRCodeState state{ GetQRCodeState(deviceID, ticket, accountData) };
        switch (state)
        {
            using enum QRCodeState;
        case Init:
        {
        }
        break;
        case Scanned:
        {
            QRCodelabel->setText("正在登录\n\n请在手机上点击「确认登录」");
        }
        break;
        case Confirmed:
        {
            QRCodelabel->setText("登录成功！");
            json::Json j;
            std::string str{ unescapeString(accountData) };
            j.parse(str);
            Account = j["uid"];
            Pwd = j["token"];
            type = 3;
            this->close();
        }
        break;
        case Expired:
        {
            emit sendQrcodeButtonVisible();
            return;
        }
        break;
        default:
            __assume(0);
        }
        Sleep(1000);
    }
}

void LoginWindow::hideQrcodeButtonFun()
{
    UpdateQrcodeButton->setVisible(false);
    controlQRCodeImage(1);
}

void LoginWindow::showQrcodeButtonFun()
{
    QRCodeImage = CV_8UC1_MatToQImage(QrcodeMat - cv::Scalar(200));
    QRCodelabel->setPixmap(QPixmap::fromImage(QRCodeImage));
    showQRcodeImage = false;
    UpdateQrcodeButton->setVisible(true);
}

void LoginWindow::getAccountPwd(std::string& Account, std::string& Pwd)
{
    Pwd = this->Pwd;
    Account = this->Account;
}

bool LoginWindow::getIsReject()
{
    return isReject;
}

void LoginWindow::showPassword(int state)
{
    if (state == Qt::Checked)
    {
        ui.lineEditPwd->setEchoMode(QLineEdit::Normal);
        ui.lineEditPwd->setInputMethodHints(Qt::ImhHiddenText);
    }
    else if (state == Qt::Unchecked)
    {
        ui.lineEditPwd->setEchoMode(QLineEdit::Password);
    }
}
