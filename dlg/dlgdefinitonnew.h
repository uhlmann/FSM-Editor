/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef DLGDEFINITONNEW_H
#define DLGDEFINITONNEW_H

#include <QDialog>

#include "ui_dlgdefinitionnew.h"

class DlgDefinitionNew : public QDialog, public Ui::DlgDefinitionNew
{
    Q_OBJECT
public:
    explicit DlgDefinitionNew(QWidget *parent = 0);

    virtual ~DlgDefinitionNew();

signals:

public slots:
  // slot to be called when name changes
  // enables/disables o.k.
  void slotNameChanged( const QString& );
  // ok is pressed - accept result
  void accept();
};

#endif // DLGDEFINITONNEW_H
