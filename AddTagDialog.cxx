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

	QHBoxLayout *secondLeftLayout = new QHBoxLayout;
	secondLeftLayout->addWidget(colorButton);
	secondLeftLayout->addWidget(colorLabel);

	QVBoxLayout *leftLayout = new QVBoxLayout;
	leftLayout->addLayout(topLeftLayout);
	leftLayout->addLayout(secondLeftLayout);
	//leftLayout->addWidget(colorButton);
	//leftLayout->addWidget(colorLabel);
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

void AddTagDialog::setTagName(std::string name){
	tagName.push_back(name);
}

void AddTagDialog::setTagColor(QColor tagQcolor){
	tagColor.push_back(tagQcolor);
}


void AddTagDialog::accept(){

	for(int i = 0; i < tagName.size(); i++){
		if(tagName[i] == lineEdit->text().toStdString()){
			QMessageBox::information(this, tr("Tag Name is Same"), "You need to choose another name for the tag");
			return;
		}

		if(tagColor[i] == color){
			QMessageBox::information(this, tr("Tag Color is Same"), "You need to choose another color for the tag");
			return;
		}
	}
	if(lineEdit->text() == NULL){
		QMessageBox::information(this, tr("Tag Name is Empty"), "You need to specify a tag name");
		return;
	}
	if(color.red() == 0 && color.green() == 0 && color.blue() == 0){
		QMessageBox::information(this, tr("Color is not chosen"), "Please choose the color of the tag");
		return;
	}
	if(!freeEdgeButton->isChecked() && !branchButton->isChecked() && !interiorButton->isChecked() && !otherButton->isChecked()){
		QMessageBox::information(this, tr("Tag type is not chosen"), "Please choose the type of the tag");
		return;
	}

	QDialog::accept();	
}


void AddTagDialog::setColor(){
	
	color = QColorDialog::getColor(Qt::green, this, "Select Color", QColorDialog::DontUseNativeDialog);

	if (color.isValid()) {
		//colorLabel->setText(color.name());
		colorLabel->setPalette(QPalette(color));
		colorLabel->setAutoFillBackground(true);
	}
}