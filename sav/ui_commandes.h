/********************************************************************************
** Form generated from reading UI file 'commandes.ui'
**
** Created by: Qt User Interface Compiler version 5.11.3
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMMANDES_H
#define UI_COMMANDES_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QLabel>

QT_BEGIN_NAMESPACE

class Ui_Commandes
{
public:
    QGridLayout *gridLayout;
    QLabel *label;

    void setupUi(QDialog *Commandes)
    {
        if (Commandes->objectName().isEmpty())
            Commandes->setObjectName(QStringLiteral("Commandes"));
        Commandes->resize(782, 408);
        gridLayout = new QGridLayout(Commandes);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label = new QLabel(Commandes);
        label->setObjectName(QStringLiteral("label"));
        QPalette palette;
        QBrush brush(QColor(245, 121, 0, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::WindowText, brush);
        palette.setBrush(QPalette::Inactive, QPalette::WindowText, brush);
        QBrush brush1(QColor(190, 190, 190, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::WindowText, brush1);
        label->setPalette(palette);
        QFont font;
        font.setPointSize(52);
        label->setFont(font);
        label->setScaledContents(false);
        label->setAlignment(Qt::AlignCenter);
        label->setTextInteractionFlags(Qt::NoTextInteraction);

        gridLayout->addWidget(label, 0, 0, 1, 1);


        retranslateUi(Commandes);

        QMetaObject::connectSlotsByName(Commandes);
    } // setupUi

    void retranslateUi(QDialog *Commandes)
    {
        Commandes->setWindowTitle(QApplication::translate("Commandes", "Dialog", nullptr));
        label->setText(QApplication::translate("Commandes", "TextLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Commandes: public Ui_Commandes {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMMANDES_H
