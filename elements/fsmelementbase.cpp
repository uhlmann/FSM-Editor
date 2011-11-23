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

FSMElementBase::FSMElementBase(FSMElementIfc* poInElementParent, QObject *poInParent, const QString& roInPrototypeName, bool bInIsPrototype )
: QObject(poInParent)
, FSMElementIfc()
, mpoElementParent( poInElementParent )
, moId()
, moDomParentId( "" )
{
  if ( bInIsPrototype)
  {// register as prototype
    // must not be allocated with new!
    getPrototypes()[ roInPrototypeName ] = this;
  }
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

// apply attributes
void FSMElementBase::applySceneAttributes( const QDomElement& /*roInElement*/ )
{

}

// returns string list with definition names
const QStringList FSMElementBase::getElementNames( void )
{
  return getPrototypes().keys();
}

// return the Id String of a dom element
QString FSMElementBase::getId( const QDomElement& roInElement )
{
  QString oResult;
  foreach (QString oProtoTypeTag, getElementNames() )
  {// search definition

    if ( roInElement.tagName() == oProtoTypeTag )
    {
      // delegate to concrete object
      oResult = (getPrototypes()[ oProtoTypeTag])->calculateId( roInElement);
      break;
    }
  }

  return oResult;
}

// calculate id from a dom element - to be implemented in concrete derived class
QString FSMElementBase::calculateId( const QDomElement& /*roInElement*/) const
{
  static unsigned int guiId = 0; // global counter
  return ( QString("id_") + QString::number( guiId++) );
}


// map with prototypes (not allocated with new!)
FSMElementBase::TagNameToElementMap_T& FSMElementBase::getPrototypes()
{
  // prototypes for creation, allocated with new
  static TagNameToElementMap_T goInstance;
  return goInstance;
}


// slot to connect on signal sigChangedId( const FSMElementIfc&)
void FSMElementBase::slotUpdateId( const FSMElementIfc& )
{
}

