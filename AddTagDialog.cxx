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

	freeEdgeButton = new QRadioButton(tr("&Free Edge Point"));
	branchButton = new QRadioButton(tr("Branch Point"));
	interiorButton = new QRadioButton(tr("Interior Point"));
	otherButton = new QRadioButton(tr("Others"));

	QHBoxLayout *topLeftLayout = new QHBoxLayout;
	topLeftLayout->addWidget(label);
	topLeftLayout->addWidget(lineEdit);

	QVBoxLayout *leftLayout = new QVBoxLayout;
    	leftLayout->addLayout(topLeftLayout);
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
}
