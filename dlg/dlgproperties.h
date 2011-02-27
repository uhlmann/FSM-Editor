/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef PROPERTIESDIALOG_H
#define PROPERTIESDIALOG_H

#include "ui_dlgproperties.h"

class Node;

class DlgProperties : public QDialog, private Ui::DlgProperties
{
    Q_OBJECT

public:
    DlgProperties(Node *poInNode, QWidget *poInParent = 0);

private slots:
    void on_buttonBox_accepted();
    void on_textColorButton_clicked();
    void on_outlineColorButton_clicked();
    void on_backgroundColorButton_clicked();

private:
    void updateColorLabel(QLabel *label, const QColor &color);
    void chooseColor(QLabel *label, QColor *color);

    Node *mpoNode;
    QColor moTextColor;
    QColor moOutlineColor;
    QColor moBackgroundColor;
};

#endif
