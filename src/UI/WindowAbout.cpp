/**
 * @file WindowAbout.cpp
 * @brief 「关于」对话框：浅色主题下展示版本信息。
 */

#include "WindowAbout.h"

WindowAbout::WindowAbout(QWidget* parent) :
    QDialog(parent)
{
    setWindowTitle("关于 MHY_Scanner");
    setFixedSize(420, 240);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(28, 28, 28, 28);
    layout->setSpacing(14);

    auto* title = new QLabel(QStringLiteral("MHY 扫码器"), this);
    title->setStyleSheet("font-size: 22px; font-weight: 700; color: #0F172A; letter-spacing: 1px;");
    title->setAlignment(Qt::AlignCenter);

    auto* body = new QLabel(
        QStringLiteral("版本 %1\nQt %2\n\n从屏幕或直播间识别米哈游游戏登录二维码")
            .arg(QStringLiteral(MHY_Scanner_VERSION))
            .arg(QStringLiteral(QT_VERSION_STR)),
        this);
    body->setStyleSheet("font-size: 13px; color: #64748B;");
    body->setAlignment(Qt::AlignCenter);
    body->setWordWrap(true);

    layout->addStretch();
    layout->addWidget(title);
    layout->addWidget(body);
    layout->addStretch();
    setLayout(layout);
}
