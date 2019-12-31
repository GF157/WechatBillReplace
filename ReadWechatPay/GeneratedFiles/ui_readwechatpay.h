/********************************************************************************
** Form generated from reading UI file 'readwechatpay.ui'
**
** Created by: Qt User Interface Compiler version 5.7.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_READWECHATPAY_H
#define UI_READWECHATPAY_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ReadWechatPayClass
{
public:
    QWidget *centralWidget;
    QGridLayout *gridLayout;
    QPushButton *pushButton;
    QLabel *label;
    QLineEdit *lineEdit_2;
    QLabel *label_2;
    QLabel *label_3;
    QLineEdit *userHeight;
    QLineEdit *lineEdit;
    QPushButton *selectButton;
    QLabel *label_4;
    QLineEdit *offsetEdit;

    void setupUi(QMainWindow *ReadWechatPayClass)
    {
        if (ReadWechatPayClass->objectName().isEmpty())
            ReadWechatPayClass->setObjectName(QStringLiteral("ReadWechatPayClass"));
        ReadWechatPayClass->resize(540, 265);
        centralWidget = new QWidget(ReadWechatPayClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout = new QGridLayout(centralWidget);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        pushButton = new QPushButton(centralWidget);
        pushButton->setObjectName(QStringLiteral("pushButton"));

        gridLayout->addWidget(pushButton, 6, 1, 1, 1);

        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit_2 = new QLineEdit(centralWidget);
        lineEdit_2->setObjectName(QStringLiteral("lineEdit_2"));

        gridLayout->addWidget(lineEdit_2, 2, 1, 1, 1);

        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        gridLayout->addWidget(label_2, 2, 0, 1, 1);

        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        gridLayout->addWidget(label_3, 3, 0, 1, 1);

        userHeight = new QLineEdit(centralWidget);
        userHeight->setObjectName(QStringLiteral("userHeight"));

        gridLayout->addWidget(userHeight, 3, 1, 1, 1);

        lineEdit = new QLineEdit(centralWidget);
        lineEdit->setObjectName(QStringLiteral("lineEdit"));

        gridLayout->addWidget(lineEdit, 0, 1, 1, 1);

        selectButton = new QPushButton(centralWidget);
        selectButton->setObjectName(QStringLiteral("selectButton"));

        gridLayout->addWidget(selectButton, 0, 2, 1, 1);

        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        gridLayout->addWidget(label_4, 4, 0, 1, 1);

        offsetEdit = new QLineEdit(centralWidget);
        offsetEdit->setObjectName(QStringLiteral("offsetEdit"));

        gridLayout->addWidget(offsetEdit, 4, 1, 1, 1);

        ReadWechatPayClass->setCentralWidget(centralWidget);

        retranslateUi(ReadWechatPayClass);

        QMetaObject::connectSlotsByName(ReadWechatPayClass);
    } // setupUi

    void retranslateUi(QMainWindow *ReadWechatPayClass)
    {
        ReadWechatPayClass->setWindowTitle(QApplication::translate("ReadWechatPayClass", "ReadWechatPay", Q_NULLPTR));
        pushButton->setText(QApplication::translate("ReadWechatPayClass", "\345\220\210\346\210\220", Q_NULLPTR));
        label->setText(QApplication::translate("ReadWechatPayClass", "\346\211\223\345\274\200\345\233\276\347\211\207:", Q_NULLPTR));
        label_2->setText(QApplication::translate("ReadWechatPayClass", "\350\276\223\345\205\245\344\277\256\346\224\271\347\232\204\346\225\260\346\215\256:", Q_NULLPTR));
        label_3->setText(QApplication::translate("ReadWechatPayClass", "\346\224\276\347\275\256\351\253\230\345\272\246:", Q_NULLPTR));
        selectButton->setText(QApplication::translate("ReadWechatPayClass", "\351\200\211\346\213\251", Q_NULLPTR));
        label_4->setText(QApplication::translate("ReadWechatPayClass", "x\350\275\264\345\201\217\347\247\273\351\207\217:", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class ReadWechatPayClass: public Ui_ReadWechatPayClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_READWECHATPAY_H
