/********************************************************************************
** Form generated from reading UI file 'mainfrm.ui'
**
** Created by: Qt User Interface Compiler version 5.13.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINFRM_H
#define UI_MAINFRM_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "gameview.h"

QT_BEGIN_NAMESPACE

class Ui_MainFrm
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *btRejouer;
    GameView *grvGame;

    void setupUi(QWidget *MainFrm)
    {
        if (MainFrm->objectName().isEmpty())
            MainFrm->setObjectName(QString::fromUtf8("MainFrm"));
        MainFrm->resize(1480, 1025);
        verticalLayout = new QVBoxLayout(MainFrm);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        btRejouer = new QPushButton(MainFrm);
        btRejouer->setObjectName(QString::fromUtf8("btRejouer"));
        btRejouer->setMaximumSize(QSize(150, 16777215));
        QFont font;
        font.setFamily(QString::fromUtf8("OCR A Extended"));
        font.setPointSize(14);
        btRejouer->setFont(font);

        verticalLayout->addWidget(btRejouer, 0, Qt::AlignHCenter);

        grvGame = new GameView(MainFrm);
        grvGame->setObjectName(QString::fromUtf8("grvGame"));

        verticalLayout->addWidget(grvGame);


        retranslateUi(MainFrm);

        QMetaObject::connectSlotsByName(MainFrm);
    } // setupUi

    void retranslateUi(QWidget *MainFrm)
    {
        MainFrm->setWindowTitle(QCoreApplication::translate("MainFrm", "Game Framework", nullptr));
        btRejouer->setText(QCoreApplication::translate("MainFrm", "Rejouer", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainFrm: public Ui_MainFrm {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINFRM_H
