/********************************************************************************
** Form generated from reading UI file 'widget.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_H
#define UI_WIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QFrame>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Widget
{
public:
    QVBoxLayout *verticalLayout;
    QFrame *frame;
    QHBoxLayout *horizontalLayout;
    QPushButton *btn_open;
    QCheckBox *checkBox_logDetail;
    QFrame *frame_2;
    QGridLayout *gridLayout;
    QLabel *label;
    QLineEdit *lineEdit_sheet1Max;
    QLabel *label_2;
    QLineEdit *lineEdit_sheet2Max;
    QSpacerItem *horizontalSpacer;
    QLabel *label_3;
    QWidget *widget;

    void setupUi(QWidget *Widget)
    {
        if (Widget->objectName().isEmpty())
            Widget->setObjectName(QString::fromUtf8("Widget"));
        Widget->resize(1366, 760);
        Widget->setMinimumSize(QSize(1366, 760));
        verticalLayout = new QVBoxLayout(Widget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        frame = new QFrame(Widget);
        frame->setObjectName(QString::fromUtf8("frame"));
        frame->setMinimumSize(QSize(0, 100));
        frame->setMaximumSize(QSize(16777215, 100));
        frame->setFrameShape(QFrame::StyledPanel);
        frame->setFrameShadow(QFrame::Raised);
        horizontalLayout = new QHBoxLayout(frame);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        btn_open = new QPushButton(frame);
        btn_open->setObjectName(QString::fromUtf8("btn_open"));

        horizontalLayout->addWidget(btn_open);

        checkBox_logDetail = new QCheckBox(frame);
        checkBox_logDetail->setObjectName(QString::fromUtf8("checkBox_logDetail"));

        horizontalLayout->addWidget(checkBox_logDetail);

        frame_2 = new QFrame(frame);
        frame_2->setObjectName(QString::fromUtf8("frame_2"));
        frame_2->setMaximumSize(QSize(273, 66));
        frame_2->setFrameShape(QFrame::StyledPanel);
        frame_2->setFrameShadow(QFrame::Raised);
        gridLayout = new QGridLayout(frame_2);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        label = new QLabel(frame_2);
        label->setObjectName(QString::fromUtf8("label"));

        gridLayout->addWidget(label, 0, 0, 1, 1);

        lineEdit_sheet1Max = new QLineEdit(frame_2);
        lineEdit_sheet1Max->setObjectName(QString::fromUtf8("lineEdit_sheet1Max"));

        gridLayout->addWidget(lineEdit_sheet1Max, 0, 1, 1, 1);

        label_2 = new QLabel(frame_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        gridLayout->addWidget(label_2, 1, 0, 1, 1);

        lineEdit_sheet2Max = new QLineEdit(frame_2);
        lineEdit_sheet2Max->setObjectName(QString::fromUtf8("lineEdit_sheet2Max"));

        gridLayout->addWidget(lineEdit_sheet2Max, 1, 1, 1, 1);


        horizontalLayout->addWidget(frame_2);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);

        label_3 = new QLabel(frame);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(0, 30));
        QFont font;
        font.setFamily(QString::fromUtf8("Microsoft JhengHei UI"));
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        label_3->setFont(font);

        horizontalLayout->addWidget(label_3);


        verticalLayout->addWidget(frame);

        widget = new QWidget(Widget);
        widget->setObjectName(QString::fromUtf8("widget"));

        verticalLayout->addWidget(widget);


        retranslateUi(Widget);

        QMetaObject::connectSlotsByName(Widget);
    } // setupUi

    void retranslateUi(QWidget *Widget)
    {
        Widget->setWindowTitle(QCoreApplication::translate("Widget", "Widget", nullptr));
        btn_open->setText(QCoreApplication::translate("Widget", "\346\211\223\345\274\200\346\226\207\344\273\266", nullptr));
        checkBox_logDetail->setText(QCoreApplication::translate("Widget", "\350\257\246\347\273\206\350\256\260\345\275\225", nullptr));
        label->setText(QCoreApplication::translate("Widget", "\350\241\2501\350\257\273\345\217\226\344\270\212\351\231\220\357\274\210\350\241\214\357\274\211\357\274\232", nullptr));
        lineEdit_sheet1Max->setText(QCoreApplication::translate("Widget", "1585", nullptr));
        label_2->setText(QCoreApplication::translate("Widget", "\350\241\2502\350\257\273\345\217\226\344\270\212\351\231\220\357\274\210\350\241\214\357\274\211\357\274\232", nullptr));
        lineEdit_sheet2Max->setText(QCoreApplication::translate("Widget", "1733", nullptr));
        label_3->setText(QCoreApplication::translate("Widget", "\345\212\240\350\275\275\344\270\255...", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Widget: public Ui_Widget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_H
