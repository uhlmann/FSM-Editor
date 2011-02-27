/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#include <QComboBox>
#include <QPushButton>

#include "dlgdefinitonnew.h"
#include "definitions/fsmdefinitionbase.h"
#include "fsmelementmanager.h"

DlgDefinitionNew::DlgDefinitionNew(QWidget *poInParent)
  : QDialog(poInParent)
  , Ui::DlgDefinitionNew()
{
  setupUi( this );

  // disable Ok button
  Ui::DlgDefinitionNew::mpoButtonBox->button( QDialogButtonBox::Ok )->setEnabled( false );

  // clear combobox
  Ui::DlgDefinitionNew::mpoComboDefinitionType->
      clear();
  // add defintion types to combobox
  Ui::DlgDefinitionNew::mpoComboDefinitionType->
      addItems( FSMDefinitionBase::getDefinitionNames());

  // observe string entered in name line edit
  connect(
      Ui::DlgDefinitionNew::mpoLineEditName
      , SIGNAL( textChanged( const QString&))
      , this
      , SLOT( slotNameChanged(const QString&)));
}

DlgDefinitionNew::~DlgDefinitionNew()
{

}

// slot to be called when name changes
// enables/disables o.k.
void DlgDefinitionNew::slotNameChanged( const QString& roInName)
{
  if ( roInName.isEmpty() || FSMElementManager::getInstance().getDefinition( roInName) != 0 )
  {
    Ui::DlgDefinitionNew::mpoButtonBox->button( QDialogButtonBox::Ok )->setEnabled( false );
  }
  else
  {
    Ui::DlgDefinitionNew::mpoButtonBox->button( QDialogButtonBox::Ok )->setEnabled( true );
  }
}

// ok is pressed - accept result
void DlgDefinitionNew::accept ()
{
  // get name of definition to create
  const QString& roTypeName = Ui::DlgDefinitionNew::mpoComboDefinitionType->currentText();
  // name
  const QString& roName     = Ui::DlgDefinitionNew::mpoLineEditName->text();
  // create by name
  FSMDefinitionBase::createByName( roTypeName, roName );

  // do work of base class - e.g. close
  QDialog::accept();
}

