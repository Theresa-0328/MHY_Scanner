#include "WindowAbout.h"

WindowAbout::WindowAbout(QWidget* parent) :
    QDialog(parent)
{
    setFixedSize(400, 200);
    QVBoxLayout* layout = new QVBoxLayout(this);
    QLabel* label = new QLabel("MHY_Scanner \n\n"
                               "版本" SCAN_VER +
                                   QString("\nQt Version: ") + QT_VERSION_STR,
                               this);
    layout->addWidget(label);
    layout->addWidget(label, 0, Qt::AlignCenter);
    setLayout(layout);
}