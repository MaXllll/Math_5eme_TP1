/********************************************************************************
** Form generated from reading UI file 'math_modelisation_2d_3d_qt.ui'
**
** Created by: Qt User Interface Compiler version 5.5.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MATH_MODELISATION_2D_3D_QT_H
#define UI_MATH_MODELISATION_2D_3D_QT_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Math_modelisation_2D_3D_QTClass
{
public:
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QWidget *centralWidget;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *Math_modelisation_2D_3D_QTClass)
    {
        if (Math_modelisation_2D_3D_QTClass->objectName().isEmpty())
            Math_modelisation_2D_3D_QTClass->setObjectName(QStringLiteral("Math_modelisation_2D_3D_QTClass"));
        Math_modelisation_2D_3D_QTClass->resize(600, 400);
        menuBar = new QMenuBar(Math_modelisation_2D_3D_QTClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        Math_modelisation_2D_3D_QTClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(Math_modelisation_2D_3D_QTClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        Math_modelisation_2D_3D_QTClass->addToolBar(mainToolBar);
        centralWidget = new QWidget(Math_modelisation_2D_3D_QTClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        Math_modelisation_2D_3D_QTClass->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(Math_modelisation_2D_3D_QTClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        Math_modelisation_2D_3D_QTClass->setStatusBar(statusBar);

        retranslateUi(Math_modelisation_2D_3D_QTClass);

        QMetaObject::connectSlotsByName(Math_modelisation_2D_3D_QTClass);
    } // setupUi

    void retranslateUi(QMainWindow *Math_modelisation_2D_3D_QTClass)
    {
        Math_modelisation_2D_3D_QTClass->setWindowTitle(QApplication::translate("Math_modelisation_2D_3D_QTClass", "Math_modelisation_2D_3D_QT", 0));
    } // retranslateUi

};

namespace Ui {
    class Math_modelisation_2D_3D_QTClass: public Ui_Math_modelisation_2D_3D_QTClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MATH_MODELISATION_2D_3D_QT_H
