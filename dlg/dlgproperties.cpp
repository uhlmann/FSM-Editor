/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#include <QtGui>

#include "elements/node.h"
#include "dlgproperties.h"

DlgProperties::DlgProperties(Node *poInNode, QWidget *poInParent)
    : QDialog(poInParent)
{
    setupUi(this);

    mpoNode = poInNode;
    mpoSpinBoxPosX->setValue(int(poInNode->x()));
    mpoSpinBoxPosY->setValue(int(poInNode->y()));
    textLineEdit->setText(poInNode->name());
    moTextColor = poInNode->textColor();
    moOutlineColor = poInNode->outlineColor();
    moBackgroundColor = poInNode->backgroundColor();

    updateColorLabel(outlineColorLabel, moOutlineColor);
    updateColorLabel(backgroundColorLabel, moBackgroundColor);
    updateColorLabel(textColorLabel, moTextColor);
}

void DlgProperties::on_buttonBox_accepted()
{
    mpoNode->setPos(mpoSpinBoxPosX->value(), mpoSpinBoxPosY->value());
    mpoNode->setName(textLineEdit->text());
    mpoNode->setOutlineColor(moOutlineColor);
    mpoNode->setBackgroundColor(moBackgroundColor);
    mpoNode->setTextColor(moTextColor);
    mpoNode->update();
    QDialog::accept();
}

void DlgProperties::on_textColorButton_clicked()
{
    chooseColor(textColorLabel, &moTextColor);
}

void DlgProperties::on_outlineColorButton_clicked()
{
    chooseColor(outlineColorLabel, &moOutlineColor);
}

void DlgProperties::on_backgroundColorButton_clicked()
{
    chooseColor(backgroundColorLabel, &moBackgroundColor);
}

void DlgProperties::updateColorLabel(QLabel *label,
                                        const QColor &color)
{
    QPixmap pixmap(16, 16);
    pixmap.fill(color);
    label->setPixmap(pixmap);
}

void DlgProperties::chooseColor(QLabel *label, QColor *color)
{
    QColor newColor = QColorDialog::getColor(*color, this);
    if (newColor.isValid()) {
        *color = newColor;
        updateColorLabel(label, *color);
    }
}
