/**
 * @file UiDialog.hpp
 * @brief 统一风格的提示/确认对话框（中文按钮 + 与主界面一致的浅色样式）。
 *
 * 避免直接使用 QMessageBox::Yes 等标准按钮导致显示英文 Yes/No。
 */

#pragma once

#include <QAbstractButton>
#include <QMessageBox>
#include <QPushButton>
#include <QString>
#include <QWidget>

namespace UiDialog
{

/**
 * @brief 信息提示框，单击「确定」关闭。
 */
inline void info(QWidget* parent, const QString& title, const QString& text)
{
    QMessageBox box(parent);
    box.setWindowTitle(title.isEmpty() ? QStringLiteral("提示") : title);
    box.setText(text);
    box.setIcon(QMessageBox::Information);
    box.addButton(QStringLiteral("确定"), QMessageBox::AcceptRole);
    box.exec();
}

/**
 * @brief 确认框。返回 true 表示用户点击了「确定」。
 */
inline bool confirm(QWidget* parent, const QString& title, const QString& text,
                    const QString& okText = QStringLiteral("确定"),
                    const QString& cancelText = QStringLiteral("取消"))
{
    QMessageBox box(parent);
    box.setWindowTitle(title.isEmpty() ? QStringLiteral("确认") : title);
    box.setText(text);
    box.setIcon(QMessageBox::Question);
    QAbstractButton* ok = box.addButton(okText, QMessageBox::YesRole);
    box.addButton(cancelText, QMessageBox::NoRole);
    box.exec();
    return box.clickedButton() == ok;
}

/**
 * @brief 警告提示框，单击「确定」关闭。
 */
inline void warn(QWidget* parent, const QString& title, const QString& text)
{
    QMessageBox box(parent);
    box.setWindowTitle(title.isEmpty() ? QStringLiteral("警告") : title);
    box.setText(text);
    box.setIcon(QMessageBox::Warning);
    box.addButton(QStringLiteral("确定"), QMessageBox::AcceptRole);
    box.exec();
}

}  // namespace UiDialog
