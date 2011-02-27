/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#include <QGraphicsItem>
#include "fsmelementbase.h"
#include "fsmelementmanager.h"

const QString FSMElementBase::gmaFSMElementTagName[ FSMElementIfc::ET_LAST ] =
{
    "state"        // ET_STATE
    , "transition" // ET_TRANSITION
    , "port"       // ET_PORT
    , "trigger"    // ET_TRIGGER
};

FSMElementBase::FSMElementBase(FSMElementIfc* poInElementParent, QObject *poInParent)
: QObject(poInParent)
, FSMElementIfc()
, mpoElementParent( poInElementParent )
, moId()
, moDomParentId( "" )
{
}

FSMElementBase::~FSMElementBase()
{
  FSMElementManager::getInstance().slotElementDestroyed( this );
}

// access to rleated dom element
QDomElement FSMElementBase::getDomElement() const
{
  return FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_FSM, const_cast<FSMElementBase*>(this) );
}


QDomElement FSMElementBase::createDomElement( QDomDocument& roInOutDomDocument ) const
{
    QString oTagName =  FSMElementBase::gmaFSMElementTagName[ getType() ];
    QDomElement oElement = roInOutDomDocument.createElement( oTagName );
    /// assign new attributes apply derived method
    updateAttributes( oElement );

    return oElement;
}


// create dom element for scene information
QDomElement FSMElementBase::createDomSceneElement( QDomDocument& roInOutDomDocument ) const
{//
  QString oTagName =  FSMElementBase::gmaFSMElementTagName[ getType() ];
  QDomElement oElement = roInOutDomDocument.createElement( oTagName );

  // assign new scene attributes apply derived methods
  updateAttributesScene( oElement);
  return oElement;
 }

// create item to be added to a QTreeWidget or returns 0 if not supported by object
QTreeWidgetItem* FSMElementBase::createTreeItem
(
   const QDomElement&  /*roInDomElement*/
   , QTreeWidgetItem * /*poInParentItem*/
   , QTreeWidget*      /*poInView*/
) const
{
    return 0; /// default not supported
}

// update dom attributes
void FSMElementBase::updateAttributes( QDomElement& /*roInOutElement*/ ) const
{

}

// assign new scene attributes apply derived methods
void FSMElementBase::updateAttributesScene( QDomElement& /*roInOutElement*/) const
{

}

// method to change id
void FSMElementBase::changedId( const QString& roInId )
{
  if ( moId == roInId ) return; /// no change --> exit

  // replace id in related elements
  FSMElementManager::getInstance().changedId( *this, roInId, moId );

  // apply changed
  moId = roInId;

  // update id in related dom element
  QDomElement oDomDummy;
  updateAttributes(      oDomDummy );
  QDomElement oDomDummy1;
  updateAttributesScene( oDomDummy1 );

  // update observers
  emit sigChangedId( *this );
}


// assign attribute - returning false if not assigned
bool FSMElementBase::setAttribute( const QString& roInName, const QString& roInValue)
{
  bool bResult = false;

  QDomElement oDomElement =
      FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_FSM, this);
  if ( !oDomElement.isNull())
  {
    bResult = true;
    oDomElement.setAttribute( roInName, roInValue );

    applyAttributes( oDomElement );

    QGraphicsItem* poGrapichsItem = getGraphicsItem();
    if ( poGrapichsItem ) poGrapichsItem->update();
  }



  return bResult;
}

// apply attributes
void FSMElementBase::applyAttributes( const QDomElement& /*roInElement*/ )
{

}

// slot to connect on signal sigChangedId( const FSMElementIfc&)
void FSMElementBase::slotUpdateId( const FSMElementIfc& )
{
}

