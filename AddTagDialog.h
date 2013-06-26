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
class QComboBox;
QT_END_NAMESPACE

//! [0]
class AddTagDialog : public QDialog
{
    Q_OBJECT

public:
    AddTagDialog(QWidget *parent = 0);

    QLabel *label;
    QLineEdit *lineEdit;
	QLabel *colorLabel;
	QColor color;
    QPushButton *addButton;
    QRadioButton *freeEdgeButton;
    QRadioButton *branchButton;
    QRadioButton *interiorButton;
    QRadioButton *otherButton;
	QComboBox *indexBox;
	QLabel *comboLabel;

	std::vector<std::string> tagName;
	std::vector<QColor> tagColor;

	int tagIndex;

	void setTagName(std::string);
	void setTagColor(QColor);

	private slots:
		void accept();
		void setColor();
		void indexChanged(int);		
};
//! [0]

#endif

