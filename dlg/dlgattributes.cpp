/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include <assert.h>

#include <QDomElement>
#include <QTableWidgetItem>
#include "dlgattributes.h"
#include "elements/fsmelementifc.h"
#include "fsmelementmanager.h"

DlgAttributes::DlgAttributes(QWidget *poInParent)
  : QWidget(poInParent)
  , mpoElement( 0 )
  , mbFilled( false )
{
  // build UI
  setupUi(this);

  // allow only complete selection of rows
  mpoTableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );

  // reactions on item changes
  connect(
      mpoTableWidget, SIGNAL( itemChanged(QTableWidgetItem*)     )
      , this,         SLOT(   slotItemChanged(QTableWidgetItem*) )  );
}

DlgAttributes::~DlgAttributes()
{
  if ( mpoTableWidget) mpoTableWidget->clear();
}

/// assosiate new element
void DlgAttributes::setElement( DomViewIfc* poInElement)
{
  if ( mpoElement != poInElement )
  {
    changedElement( poInElement );
  }
}

// clear Table and set dimesion to (0,0)
void DlgAttributes::clear()
{
  if ( !mpoTableWidget ) return;

  // erase rows
  mpoTableWidget->setRowCount( 0 );
}

// actions to be applied when element changes
void DlgAttributes::changedElement( DomViewIfc* poInElement )
{
  if ( mpoElement )
  {
    // release current element
    releaseElement( mpoElement );
  }

  // assign new element
  assignElement( poInElement );
}

// actions to be applied when new element is set
void DlgAttributes::assignElement( DomViewIfc* poInElement )
{
  mpoElement = poInElement;

  // assign new element
  mpoElement = poInElement;

  if ( mpoElement )
  {
    // create connections
    connect(
        mpoElement->getSignalSlotBase(), SIGNAL( destroyed( QObject* ) )
        , this,     SLOT( slotDestroyed( QObject* ) ) );

    // display attributes
    // related dom element - initalized isNull() is true
    QDomElement   oDomElement = mpoElement->getDomElement();

    // show attributes or clear table
    setDomElement( oDomElement );
  } // if mpoElement
  else
  {
    assert( !mpoElement );

    // remove rows
    clear();
  }
}

// actions to be applied when element is released
void DlgAttributes::releaseElement( DomViewIfc* poInElement )
{
  if ( !poInElement ) return;

  // release connections to this object
  QObject* poObject = poInElement->getSignalSlotBase();
  if ( poObject )
  {
    poObject->disconnect( this );
  }
}

// display dom elements
void DlgAttributes::setDomElement( const QDomElement& roInElement )
{
  mbFilled = false;
  if ( roInElement.isNull() )
  {
    // clear table
    clear();
  }
  else
  {

    const QDomNamedNodeMap& roAttributes = roInElement.attributes();
    mpoTableWidget->setRowCount( roInElement.attributes().count() );

    int iRow = 0;
    int iCol = 0;
    for ( iRow = 0; iRow < roAttributes.count();++iRow  )
    {
      // attribute with index iRow
      QDomAttr oAttribute = roAttributes.item( iRow ).toAttr();

      if ( oAttribute.isNull() ) continue;

      // fill attributes into the table widget
      for ( iCol = 0; iCol < 2; ++iCol )
      {
        // item in table
        QTableWidgetItem* poItem = mpoTableWidget->item( iRow, iCol );
        if ( !poItem )
        {
          poItem = new QTableWidgetItem;
        }

        if ( iCol == 0 )
        {// assign attribute name
          poItem->setText( oAttribute.name() );
          // name must not be edited
          Qt::ItemFlags eFlags = poItem->flags();
          eFlags &= ~Qt::ItemIsEditable;
          poItem->setFlags( eFlags );
        }
        else
        {
          poItem->setText( oAttribute.value() );
        }
        mpoTableWidget->setItem( iRow, iCol, poItem);
      }
    }
  }

  mbFilled = true;
}

// slot to handle change of attributes in table
void DlgAttributes::slotItemChanged ( QTableWidgetItem * poInItem )
{
  assert( poInItem );

  if ( !mbFilled )               return; /// filling in progress
  if ( poInItem->column() == 0 ) return; /// attribute name

  if ( mpoTableWidget && mpoElement )
  {
    assert( poInItem->column() > 0 );
    QTableWidgetItem* poNameItem = mpoTableWidget->item( poInItem->row(), poInItem->column()-1);
    // assign attribute to element
    mpoElement->setAttribute( poNameItem->text(), poInItem->text());
  }
}

// slot to handle destruction of assosiated element
void DlgAttributes::slotDestroyed( QObject* )
{
  mpoElement = 0;

  // remove rows
  clear();

}



