#include "WindowLogin.h"

WindowLogin::WindowLogin(QWidget* parent) :
    QWidget(parent)
{
    //setWindowFlags(Qt::Dialog);
    setWindowModality(Qt::WindowModal);
    setMinimumSize(QSize(450, 450));
    setMaximumSize(QSize(450, 450));
}

WindowLogin::~WindowLogin()
{
}