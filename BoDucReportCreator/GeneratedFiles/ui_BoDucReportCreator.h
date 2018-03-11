/********************************************************************************
** Form generated from reading UI file 'BoDucReportCreator.ui'
**
** Created by: Qt User Interface Compiler version 5.8.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_BODUCREPORTCREATOR_H
#define UI_BODUCREPORTCREATOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_BoDucReportCreatorClass
{
public:

    void setupUi(QWidget *BoDucReportCreatorClass)
    {
        if (BoDucReportCreatorClass->objectName().isEmpty())
            BoDucReportCreatorClass->setObjectName(QStringLiteral("BoDucReportCreatorClass"));
        BoDucReportCreatorClass->resize(600, 400);

        retranslateUi(BoDucReportCreatorClass);

        QMetaObject::connectSlotsByName(BoDucReportCreatorClass);
    } // setupUi

    void retranslateUi(QWidget *BoDucReportCreatorClass)
    {
        BoDucReportCreatorClass->setWindowTitle(QApplication::translate("BoDucReportCreatorClass", "BoDucReportCreator", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class BoDucReportCreatorClass: public Ui_BoDucReportCreatorClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_BODUCREPORTCREATOR_H
