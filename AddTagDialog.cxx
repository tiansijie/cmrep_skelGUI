#include <QtGui>

#include "AddTagDialog.h"

AddTagDialog::AddTagDialog(QWidget *parent)
	: QDialog(parent)
{
	label = new QLabel(tr("Add Tag:"));
	lineEdit = new QLineEdit;
	label->setBuddy(lineEdit);

	addButton = new QPushButton(tr("&Add"));
	addButton->setDefault(true);

	colorLabel = new QLabel;
	//colorLabel->setFrameStyle(frameStyle);
	QPushButton *colorButton = new QPushButton(tr("Tag &Color"));

	freeEdgeButton = new QRadioButton(tr("&Free Edge Point"));
	branchButton = new QRadioButton(tr("&Branch Point"));
	interiorButton = new QRadioButton(tr("&Interior Point"));
	otherButton = new QRadioButton(tr("&Others"));

	QHBoxLayout *topLeftLayout = new QHBoxLayout;
	topLeftLayout->addWidget(label);
	topLeftLayout->addWidget(lineEdit);

	QVBoxLayout *leftLayout = new QVBoxLayout;
	leftLayout->addLayout(topLeftLayout);
	leftLayout->addWidget(colorButton);
	leftLayout->addWidget(colorLabel);
    leftLayout->addWidget(freeEdgeButton);
    leftLayout->addWidget(branchButton);
    leftLayout->addWidget(interiorButton);
	leftLayout->addWidget(otherButton);
	leftLayout->addWidget(addButton);

	QGridLayout *mainLayout = new QGridLayout;
	mainLayout->addLayout(leftLayout, 0, 0);
	mainLayout->setRowStretch(2, 1);
	setLayout(mainLayout);
	setWindowTitle(tr("Add Tag"));	

	connect(addButton, SIGNAL(clicked()), this, SLOT(accept()));
	connect(colorButton, SIGNAL(clicked()), this, SLOT(setColor()));
}


void AddTagDialog::setColor(){
	
/*
	if (native->isChecked())
		color = QColorDialog::getColor(Qt::green, this);
	else*/
	color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);

	if (color.isValid()) {
		colorLabel->setText(color.name());
		colorLabel->setPalette(QPalette(color));
		colorLabel->setAutoFillBackground(true);
	}
}