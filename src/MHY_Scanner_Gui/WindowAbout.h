#pragma once

#include <QLabel>
#include <QVBoxLayout>
#include <QDialog>

class WindowAbout : public QDialog
{
    Q_OBJECT

public:
    WindowAbout(QWidget* parent = nullptr);
};