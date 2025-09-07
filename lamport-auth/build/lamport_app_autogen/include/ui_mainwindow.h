/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.9.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralWidget;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *topRow;
    QPushButton *btnConnect;
    QPushButton *btnStart;
    QPushButton *btnStop;
    QPushButton *btnDisconnect;
    QSpacerItem *horizontalSpacer;
    QLabel *lblStatus;
    QLabel *lblRound;
    QTextEdit *txtLog;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(780, 520);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName("centralWidget");
        verticalLayout = new QVBoxLayout(centralWidget);
        verticalLayout->setObjectName("verticalLayout");
        topRow = new QHBoxLayout();
        topRow->setObjectName("topRow");
        btnConnect = new QPushButton(centralWidget);
        btnConnect->setObjectName("btnConnect");

        topRow->addWidget(btnConnect);

        btnStart = new QPushButton(centralWidget);
        btnStart->setObjectName("btnStart");

        topRow->addWidget(btnStart);

        btnStop = new QPushButton(centralWidget);
        btnStop->setObjectName("btnStop");

        topRow->addWidget(btnStop);

        btnDisconnect = new QPushButton(centralWidget);
        btnDisconnect->setObjectName("btnDisconnect");

        topRow->addWidget(btnDisconnect);

        horizontalSpacer = new QSpacerItem(0, 0, QSizePolicy::Policy::Expanding, QSizePolicy::Policy::Minimum);

        topRow->addItem(horizontalSpacer);

        lblStatus = new QLabel(centralWidget);
        lblStatus->setObjectName("lblStatus");

        topRow->addWidget(lblStatus);

        lblRound = new QLabel(centralWidget);
        lblRound->setObjectName("lblRound");

        topRow->addWidget(lblRound);


        verticalLayout->addLayout(topRow);

        txtLog = new QTextEdit(centralWidget);
        txtLog->setObjectName("txtLog");
        txtLog->setReadOnly(true);

        verticalLayout->addWidget(txtLog);

        MainWindow->setCentralWidget(centralWidget);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "Lamport Auth Demo", nullptr));
        btnConnect->setText(QCoreApplication::translate("MainWindow", "Connect", nullptr));
        btnStart->setText(QCoreApplication::translate("MainWindow", "Start", nullptr));
        btnStop->setText(QCoreApplication::translate("MainWindow", "Stop", nullptr));
        btnDisconnect->setText(QCoreApplication::translate("MainWindow", "Disconnect", nullptr));
        lblStatus->setText(QCoreApplication::translate("MainWindow", "Status: Idle", nullptr));
        lblRound->setText(QCoreApplication::translate("MainWindow", "Round: 0 / 0", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
