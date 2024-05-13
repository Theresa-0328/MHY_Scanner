#include "LoginWindow.h"

#include <QRegularExpressionValidator>
#include <iostream>

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
        //std::cout << threadPool.activeThreadCount() << std::endl;
        threadPool.clear();
        threadPool.start([&]() {showQRcodeImage = true; LoginWindow::qrcodeThreadFun(index); });
    }
    break;
    case 2:
    {
    }
        break;
    default:
        break;
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

cv::Mat LoginWindow::createQrCodeToCvMat(const std::string_view qrcodeString) const
{
    qrcodegen::QrCode qr{ qrcodegen::QrCode::encodeText(qrcodeString.data(), qrcodegen::QrCode::Ecc::QUARTILE) };
    int size{ qr.getSize() };
    int scale{ 5 };
    int padding{ 0 };
    cv::Mat qrImage(size * scale + padding * 2, size * scale + padding * 2, CV_8UC1, cv::Scalar(255));
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            bool isDark = qr.getModule(x, y);
            for (int i = 0; i < scale; i++)
            {
                for (int j = 0; j < scale; j++)
                {
                    qrImage.at<uchar>(y * scale + i + padding, x * scale + j + padding) = isDark ? 0 : 255;
                }
            }
        }
    }
    //cv::imshow("QR Code", qrImage);
    //cv::waitKey(0);
    return qrImage;
}

void LoginWindow::qrcodeThreadFun(int index)
{
    enum class QRCodeState
    {
        Init = 0,
        Scanned = 1,
        Confirmed = 2,
        Expired = 3
    };
    Sleep(1500); //模拟获取二维码访问接口时间
    QrcodeMat = createQrCodeToCvMat(qrcodeString);
    QRCodeImage = Mat2QImage(QrcodeMat);
    QRCodelabel->setPixmap(QPixmap::fromImage(QRCodeImage));
    Sleep(1500);
    int kk{ 5 };
    while (showQRcodeImage && kk > 0)
    {
        kk--;
        std::cout << __LINE__ << "---" << timer.elapsed() << " seconds" << std::endl;
        QRCodeState state = static_cast<QRCodeState>(0);
        if (kk < 3)
            state = static_cast<QRCodeState>(1);
        //访问接口
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
            break;
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
    QRCodeImage = Mat2QImage(QrcodeMat - cv::Scalar(200));
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
