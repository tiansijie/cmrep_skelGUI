#ifndef ADDTAGDIALOG_H
#define ADDTAGDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
//class QCheckBox;
class QDialogButtonBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
QT_END_NAMESPACE

//! [0]
class AddTagDialog : public QDialog
{
    Q_OBJECT

public:
    AddTagDialog(QWidget *parent = 0);

//private:
    QLabel *label;
    QLineEdit *lineEdit;
	QLabel *colorLabel;
	QColor color;
   /* QCheckBox *caseCheckBox;
    QCheckBox *fromStartCheckBox;
    QCheckBox *wholeWordsCheckBox;
    QCheckBox *searchSelectionCheckBox;
    QCheckBox *backwardCheckBox;
    QDialogButtonBox *buttonBox;*/
    QPushButton *addButton;
    //QPushButton *moreButton;
    QRadioButton *freeEdgeButton;
    QRadioButton *branchButton;
    QRadioButton *interiorButton;
    QRadioButton *otherButton;
	

	std::vector<std::string> tagName;
	std::vector<QColor> tagColor;

	void setTagName(std::string);
	void setTagColor(QColor);


	private slots:
		void accept();
		void setColor();
		
};
//! [0]

#endif

