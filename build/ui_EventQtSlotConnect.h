/********************************************************************************
** Form generated from reading UI file 'EventQtSlotConnect.ui'
**
** Created: Thu Jun 6 17:56:34 2013
**      by: Qt User Interface Compiler version 4.8.4
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_EVENTQTSLOTCONNECT_H
#define UI_EVENTQTSLOTCONNECT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QFrame>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTextEdit>
#include <QtGui/QWidget>
#include "QVTKWidget.h"

QT_BEGIN_NAMESPACE

class Ui_EventQtSlotConnect
{
public:
    QAction *actionOpenFile;
    QAction *actionExit;
    QAction *actionPrint;
    QAction *actionHelp;
    QAction *actionSave;
    QAction *actionOpen;
    QAction *actionSave_2;
    QWidget *centralwidget;
    QVTKWidget *qvtkWidget;
    QTextEdit *textEdit;
    QTabWidget *tabWidget;
    QWidget *tab;
    QPushButton *cmrepVskel;
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QSpinBox *spinBox;
    QDoubleSpinBox *doubleSpinBox;
    QSpinBox *spinBox_2;
    QSpinBox *spinBox_3;
    QFrame *line;
    QFrame *line_2;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QLineEdit *lineEdit;
    QLabel *label_8;
    QLineEdit *lineEdit_2;
    QLabel *label_9;
    QLineEdit *lineEdit_3;
    QLabel *label_10;
    QLineEdit *lineEdit_4;
    QLabel *label_11;
    QLineEdit *lineEdit_5;
    QLabel *label_12;
    QLabel *label_13;
    QLineEdit *lineEdit_6;
    QWidget *tab_2;
    QWidget *tab_3;
    QRadioButton *radioButtonVertex;
    QRadioButton *radioButtonBranch;
    QGroupBox *groupBox;
    QRadioButton *radioButtonSurface;

    void setupUi(QMainWindow *EventQtSlotConnect)
    {
        if (EventQtSlotConnect->objectName().isEmpty())
            EventQtSlotConnect->setObjectName(QString::fromUtf8("EventQtSlotConnect"));
        EventQtSlotConnect->resize(798, 660);
        actionOpenFile = new QAction(EventQtSlotConnect);
        actionOpenFile->setObjectName(QString::fromUtf8("actionOpenFile"));
        actionOpenFile->setEnabled(true);
        actionExit = new QAction(EventQtSlotConnect);
        actionExit->setObjectName(QString::fromUtf8("actionExit"));
        actionPrint = new QAction(EventQtSlotConnect);
        actionPrint->setObjectName(QString::fromUtf8("actionPrint"));
        actionHelp = new QAction(EventQtSlotConnect);
        actionHelp->setObjectName(QString::fromUtf8("actionHelp"));
        actionSave = new QAction(EventQtSlotConnect);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionOpen = new QAction(EventQtSlotConnect);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        actionSave_2 = new QAction(EventQtSlotConnect);
        actionSave_2->setObjectName(QString::fromUtf8("actionSave_2"));
        centralwidget = new QWidget(EventQtSlotConnect);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        qvtkWidget = new QVTKWidget(centralwidget);
        qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));
        qvtkWidget->setEnabled(true);
        qvtkWidget->setGeometry(QRect(20, 30, 521, 491));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(qvtkWidget->sizePolicy().hasHeightForWidth());
        qvtkWidget->setSizePolicy(sizePolicy);
        qvtkWidget->setAutoFillBackground(true);
        textEdit = new QTextEdit(centralwidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));
        textEdit->setGeometry(QRect(30, 600, 301, 51));
        tabWidget = new QTabWidget(centralwidget);
        tabWidget->setObjectName(QString::fromUtf8("tabWidget"));
        tabWidget->setGeometry(QRect(570, 30, 211, 451));
        tab = new QWidget();
        tab->setObjectName(QString::fromUtf8("tab"));
        tab->setEnabled(true);
        cmrepVskel = new QPushButton(tab);
        cmrepVskel->setObjectName(QString::fromUtf8("cmrepVskel"));
        cmrepVskel->setGeometry(QRect(50, 390, 101, 31));
        label = new QLabel(tab);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 100, 54, 12));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 130, 54, 12));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 160, 54, 12));
        label_4 = new QLabel(tab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(10, 190, 54, 12));
        spinBox = new QSpinBox(tab);
        spinBox->setObjectName(QString::fromUtf8("spinBox"));
        spinBox->setGeometry(QRect(40, 100, 42, 22));
        doubleSpinBox = new QDoubleSpinBox(tab);
        doubleSpinBox->setObjectName(QString::fromUtf8("doubleSpinBox"));
        doubleSpinBox->setGeometry(QRect(40, 130, 62, 22));
        spinBox_2 = new QSpinBox(tab);
        spinBox_2->setObjectName(QString::fromUtf8("spinBox_2"));
        spinBox_2->setGeometry(QRect(40, 160, 42, 22));
        spinBox_3 = new QSpinBox(tab);
        spinBox_3->setObjectName(QString::fromUtf8("spinBox_3"));
        spinBox_3->setGeometry(QRect(40, 190, 42, 22));
        line = new QFrame(tab);
        line->setObjectName(QString::fromUtf8("line"));
        line->setGeometry(QRect(-10, 210, 221, 20));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);
        line_2 = new QFrame(tab);
        line_2->setObjectName(QString::fromUtf8("line_2"));
        line_2->setGeometry(QRect(-10, 50, 221, 20));
        line_2->setFrameShape(QFrame::HLine);
        line_2->setFrameShadow(QFrame::Sunken);
        label_5 = new QLabel(tab);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(50, 60, 101, 21));
        label_6 = new QLabel(tab);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(60, 220, 91, 16));
        label_7 = new QLabel(tab);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(10, 240, 54, 12));
        lineEdit = new QLineEdit(tab);
        lineEdit->setObjectName(QString::fromUtf8("lineEdit"));
        lineEdit->setGeometry(QRect(50, 240, 113, 20));
        label_8 = new QLabel(tab);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(10, 270, 54, 12));
        lineEdit_2 = new QLineEdit(tab);
        lineEdit_2->setObjectName(QString::fromUtf8("lineEdit_2"));
        lineEdit_2->setGeometry(QRect(50, 270, 113, 20));
        label_9 = new QLabel(tab);
        label_9->setObjectName(QString::fromUtf8("label_9"));
        label_9->setGeometry(QRect(10, 300, 54, 12));
        lineEdit_3 = new QLineEdit(tab);
        lineEdit_3->setObjectName(QString::fromUtf8("lineEdit_3"));
        lineEdit_3->setGeometry(QRect(50, 300, 113, 20));
        label_10 = new QLabel(tab);
        label_10->setObjectName(QString::fromUtf8("label_10"));
        label_10->setGeometry(QRect(10, 330, 54, 12));
        lineEdit_4 = new QLineEdit(tab);
        lineEdit_4->setObjectName(QString::fromUtf8("lineEdit_4"));
        lineEdit_4->setGeometry(QRect(50, 330, 113, 20));
        label_11 = new QLabel(tab);
        label_11->setObjectName(QString::fromUtf8("label_11"));
        label_11->setGeometry(QRect(10, 360, 54, 12));
        lineEdit_5 = new QLineEdit(tab);
        lineEdit_5->setObjectName(QString::fromUtf8("lineEdit_5"));
        lineEdit_5->setGeometry(QRect(50, 360, 113, 20));
        label_12 = new QLabel(tab);
        label_12->setObjectName(QString::fromUtf8("label_12"));
        label_12->setGeometry(QRect(50, 0, 101, 21));
        label_13 = new QLabel(tab);
        label_13->setObjectName(QString::fromUtf8("label_13"));
        label_13->setGeometry(QRect(10, 30, 54, 12));
        lineEdit_6 = new QLineEdit(tab);
        lineEdit_6->setObjectName(QString::fromUtf8("lineEdit_6"));
        lineEdit_6->setGeometry(QRect(50, 30, 113, 20));
        tabWidget->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QString::fromUtf8("tab_2"));
        tabWidget->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QString::fromUtf8("tab_3"));
        tabWidget->addTab(tab_3, QString());
        radioButtonVertex = new QRadioButton(centralwidget);
        radioButtonVertex->setObjectName(QString::fromUtf8("radioButtonVertex"));
        radioButtonVertex->setGeometry(QRect(580, 520, 89, 16));
        radioButtonBranch = new QRadioButton(centralwidget);
        radioButtonBranch->setObjectName(QString::fromUtf8("radioButtonBranch"));
        radioButtonBranch->setGeometry(QRect(580, 540, 89, 16));
        groupBox = new QGroupBox(centralwidget);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(570, 499, 120, 81));
        radioButtonSurface = new QRadioButton(groupBox);
        radioButtonSurface->setObjectName(QString::fromUtf8("radioButtonSurface"));
        radioButtonSurface->setGeometry(QRect(10, 60, 89, 16));
        EventQtSlotConnect->setCentralWidget(centralwidget);
        tabWidget->raise();
        groupBox->raise();
        qvtkWidget->raise();
        textEdit->raise();
        radioButtonVertex->raise();
        radioButtonBranch->raise();

        retranslateUi(EventQtSlotConnect);

        tabWidget->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(EventQtSlotConnect);
    } // setupUi

    void retranslateUi(QMainWindow *EventQtSlotConnect)
    {
        EventQtSlotConnect->setWindowTitle(QApplication::translate("EventQtSlotConnect", "EventQtSlotConnect", 0, QApplication::UnicodeUTF8));
        actionOpenFile->setText(QApplication::translate("EventQtSlotConnect", "Open File...", 0, QApplication::UnicodeUTF8));
        actionExit->setText(QApplication::translate("EventQtSlotConnect", "Exit", 0, QApplication::UnicodeUTF8));
        actionPrint->setText(QApplication::translate("EventQtSlotConnect", "Print", 0, QApplication::UnicodeUTF8));
        actionHelp->setText(QApplication::translate("EventQtSlotConnect", "Help", 0, QApplication::UnicodeUTF8));
        actionSave->setText(QApplication::translate("EventQtSlotConnect", "Save", 0, QApplication::UnicodeUTF8));
        actionOpen->setText(QApplication::translate("EventQtSlotConnect", "Open", 0, QApplication::UnicodeUTF8));
        actionSave_2->setText(QApplication::translate("EventQtSlotConnect", "Save", 0, QApplication::UnicodeUTF8));
        cmrepVskel->setText(QApplication::translate("EventQtSlotConnect", "Get Skeleton", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("EventQtSlotConnect", "e", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("EventQtSlotConnect", "p", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("EventQtSlotConnect", "c", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("EventQtSlotConnect", "t", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("EventQtSlotConnect", "Pruning Options", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("EventQtSlotConnect", "Output Options", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("EventQtSlotConnect", "s", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("EventQtSlotConnect", "R", 0, QApplication::UnicodeUTF8));
        label_9->setText(QApplication::translate("EventQtSlotConnect", "T", 0, QApplication::UnicodeUTF8));
        label_10->setText(QApplication::translate("EventQtSlotConnect", "I", 0, QApplication::UnicodeUTF8));
        label_11->setText(QApplication::translate("EventQtSlotConnect", "q", 0, QApplication::UnicodeUTF8));
        label_12->setText(QApplication::translate("EventQtSlotConnect", "General Options", 0, QApplication::UnicodeUTF8));
        label_13->setText(QApplication::translate("EventQtSlotConnect", "Path", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab), QApplication::translate("EventQtSlotConnect", "Cmrep Skeleton", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_2), QApplication::translate("EventQtSlotConnect", "Tab 2", 0, QApplication::UnicodeUTF8));
        tabWidget->setTabText(tabWidget->indexOf(tab_3), QApplication::translate("EventQtSlotConnect", "Page", 0, QApplication::UnicodeUTF8));
        radioButtonVertex->setText(QApplication::translate("EventQtSlotConnect", "Vertex", 0, QApplication::UnicodeUTF8));
        radioButtonBranch->setText(QApplication::translate("EventQtSlotConnect", "Branch Point", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("EventQtSlotConnect", "GroupBox", 0, QApplication::UnicodeUTF8));
        radioButtonSurface->setText(QApplication::translate("EventQtSlotConnect", "Surface Point", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class EventQtSlotConnect: public Ui_EventQtSlotConnect {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_EVENTQTSLOTCONNECT_H
