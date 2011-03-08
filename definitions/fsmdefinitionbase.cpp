/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include <QDomElement>

#include "fsmdefinitionbase.h"
#include "fsmelementmanager.h"

// name of common attributes
const QString FSMDefinitionBase::moAttributeNames[ FSMDefinitionBase::AN_LAST ] =
{
  QString( "name")
    };

FSMDefinitionBase::FSMDefinitionBase
(
    const QString&     roInName
    , const QString&   roInTypeName
    , DefinitionType_T eInType
    , bool             bInIsPrototype
    , DefinitionFlag_T eInFlags
)
: FSMDefinitionIfc()
, meFlags( eInFlags )
, moTypeName( roInTypeName )
, meType( eInType )
{
  // clear Strings
  unsigned int uiIdx = 0;
  for ( uiIdx = 0; uiIdx < AN_LAST; ++uiIdx)
  {
    moAttributeValues[ uiIdx ] = "";
  }

  moAttributeValues[ AN_NAME ] = roInName;

  if ( bInIsPrototype )
  {// use element as prototyp
    // must not be allocated with new!
    getPrototypes()[ moTypeName ] = this;
  }
  else
  {// no prototype - real new definition object
    if ( !( meFlags & DF_UNMANAGED) )
    {// element shall register itself
      FSMElementManager::getInstance().addDefinition( const_cast<FSMDefinitionBase*>( this ) );
    }
  }
}


// virtual destructor
FSMDefinitionBase::~FSMDefinitionBase()
{
}

// create dom element in current working document
QDomElement FSMDefinitionBase::createDomElement( QDomDocument& roInOutDomDocument ) const
{
  QDomElement oElement = roInOutDomDocument.createElement( moTypeName );

  return oElement;
}


// create an element related to a tag name
FSMDefinitionIfc* FSMDefinitionBase::createByName( const QString& roInTypeName, const QString& roInName )
{
  if ( getPrototypes().contains( roInTypeName ) )
  {
    const FSMDefinitionBase* poPrototype =  getPrototypes()[ roInTypeName ];
    // clone and assign new name
    FSMDefinitionBase* poDefinitionNew = poPrototype->clone( roInName);

    if ( poDefinitionNew )
    {
      QDomElement oDomDummy;
      poDefinitionNew->updateAttributes( oDomDummy );
      return poDefinitionNew;
    }
  }

  return 0;
}

// returns string list with definition names
const QStringList FSMDefinitionBase::getDefinitionNames( void )
{
  return getPrototypes().keys();
}

// return attribute name related to AN_ with number uiIdx < AN_LAST
QString      FSMDefinitionBase::getAttributeName( unsigned int uiIdx ) const
{
  if ( uiIdx >= AN_LAST ) return QString();

  return moAttributeNames[ uiIdx ];
}
unsigned int FSMDefinitionBase::getAttributeCount() const
{
  return AN_LAST;
}

// access to rleated dom element
QDomElement FSMDefinitionBase::getDomElement() const
{
  return FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_DEFINITIONS, const_cast<FSMDefinitionBase*>(this) );
}
// assign attribute - returning false if not assigned
bool FSMDefinitionBase::setAttribute( const QString& roInName, const QString& roInValue)
{
  bool bResult = false;

  QDomElement oDomElement =
      FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_DEFINITIONS, this);
  if ( !oDomElement.isNull())
  {
    bResult = true;
    oDomElement.setAttribute( roInName, roInValue );

    applyAttributes( oDomElement );
  }

  return bResult;
}



// map with prototypes (not allocated with new!)
FSMDefinitionBase::TagNameToDefinitionMap_T& FSMDefinitionBase::getPrototypes()
{
  // prototypes for creation, allocated with new
  static TagNameToDefinitionMap_T goInstance;
  return goInstance;
}

// apply attributes
void FSMDefinitionBase::applyAttributes( const QDomElement& roInElement )
{
  applyAttributes( roInElement, moAttributeNames, moAttributeValues, AN_LAST );
}

// apply attributes and assign values in an arry
void FSMDefinitionBase::applyAttributes
(
  const QDomElement& roInDefinition
  , const QString*   paoInAttributeNames
  , QString*         paoInOutAttributeValues
  , unsigned int     uiInSize
  , unsigned int     uiInIdxId
)
{
  if ( !paoInAttributeNames || !paoInOutAttributeValues ) return;
  unsigned int uiIdx = 0;
  for ( uiIdx = 0; uiIdx < uiInSize; ++uiIdx )
  {
    QString oAttr = roInDefinition.attribute( paoInAttributeNames[ uiIdx ], "" );
    if ( uiIdx != uiInIdxId )
    {// assign attribute value
      paoInOutAttributeValues[ uiIdx ] = oAttr;
    }
  }

  if ( uiInIdxId < uiInSize )
  {
    changedId( roInDefinition.attribute( paoInAttributeNames[ uiInIdxId ], "" ) );
  }
}

// update attributes in related QDomElement
void FSMDefinitionBase::updateAttributes
(
    QDomElement&    roInOutDefinition
    , const QString* paoInAttributeNames
    , const QString* paoInAttributeValues
    , unsigned int  uiInArraySize
) const
{
  if ( !paoInAttributeNames || !paoInAttributeValues) return;

  if (roInOutDefinition.isNull() )
  {
    roInOutDefinition =
        FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_DEFINITIONS, const_cast<FSMDefinitionBase*>( this ) );
  }

  if ( roInOutDefinition.isNull() ) return;

  unsigned int uiIdx = 0;
  for ( uiIdx = 0; uiIdx < uiInArraySize; ++uiIdx )
  {
    // assign name
    roInOutDefinition.setAttribute( paoInAttributeNames[uiIdx], paoInAttributeValues[ uiIdx ] );
  }
}

// update dom attributes - to be overwritten in derived classes
void FSMDefinitionBase::updateAttributes( QDomElement& roInOutDefinition ) const
{
  updateAttributes( roInOutDefinition, moAttributeNames, moAttributeValues, AN_LAST );
}


// method to change id
void FSMDefinitionBase::changedId( const QString& roInId )
{
  if ( !roInId.isEmpty())
  {
    if ( moAttributeValues[ AN_NAME ] == roInId ) return; /// no change --> exit

    // replace id in related elements
    FSMElementManager::getInstance().changedId( *this, roInId, moAttributeValues[ AN_NAME ] );

    // apply change
    moAttributeValues[ AN_NAME ] = roInId;
  }
  else
  {
    // replace id in related elements
    FSMElementManager::getInstance().changedId( *this, moAttributeValues[ AN_NAME ], moAttributeValues[ AN_NAME ] );

  }




  // update id in related dom element
  QDomElement oDomDummy;
  updateAttributes(      oDomDummy );

  // update observers
  emit sigChangedId( *this );
}



