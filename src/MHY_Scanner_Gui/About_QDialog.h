#pragma once

#include <QLabel>
#include <QVBoxLayout>
#include <QtWidgets/QDialog>

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    AboutDialog(QDialog* parent = nullptr);
};