#include "About_QDialog.h"

AboutDialog::AboutDialog(QDialog* parent)
	: QDialog(parent)
{
	setFixedSize(400, 200);
	QVBoxLayout* layout = new QVBoxLayout(this);
	QLabel* label = new QLabel("", this);
	layout->addWidget(label);
	setLayout(layout);
}