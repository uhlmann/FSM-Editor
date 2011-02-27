/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include "fsminclude.h"
#include "fsmelementmanager.h"

using namespace FSMDefinition;

// name of common attributes
const QString FSMInclude::moAttributeNames[ FSMInclude::AN_LAST ] =
{
  QString( "file") // AN_FILE
};

// register prototype
static FSMInclude goIncludePrototype( "proto_include", true );

FSMInclude::FSMInclude( const QString& roInName, bool bInIsPrototype)
  : FSMDefinitionBase( roInName, "include", DT_INCLUDE, bInIsPrototype, DF_UNMANAGED )
  , moFile()
{
  // clear Strings
  unsigned int uiIdx = 0;
  for ( uiIdx = 0; uiIdx < AN_LAST-FSMDefinitionBase::AN_LAST; ++uiIdx)
  {
    moAttributeValues[ uiIdx ] = "";
  }

  if ( !bInIsPrototype)
  { // prototypes must not be registered
    // register element at manager - not done in base class du to flag DF_UNMANAGER
    FSMElementManager::getInstance().addDefinition( const_cast<FSMInclude*>( this ) );
  }
}

// creation method
FSMInclude* FSMInclude::clone( const QString& roInName ) const
{
  return new FSMInclude( roInName );
}

/* overwritten of base class */
// return attribute name related to AN_ with number uiIdx < AN_LAST
QString  FSMInclude::getAttributeName( unsigned int uiIdx ) const
{
  if ( uiIdx >= AN_LAST ) return QString();

  if ( uiIdx < FSMDefinitionBase::AN_LAST )
  {
    return FSMDefinitionBase::moAttributeNames[ uiIdx ];
  }
  else
  {
    return moAttributeNames[ uiIdx-FSMDefinitionBase::AN_LAST ];
  }
}

QString FSMInclude::getAttributeValue( unsigned int uiIdx ) const
{
  if ( uiIdx >= AN_LAST ) return QString();

  return moAttributeValues[ uiIdx-FSMDefinitionBase::AN_LAST ];
}


bool FSMInclude::setAttributeValue( unsigned int uiIdx, const QString& roInValue )
{
  if ( uiIdx >= AN_LAST) return false;

  moAttributeValues[ uiIdx - FSMDefinitionBase::AN_LAST ] = roInValue;

  return true;
}

// apply attributes
void FSMInclude::applyAttributes( const QDomElement& roInDefinition )
{

  // handle parameters of this class
  FSMDefinitionBase::applyAttributes( roInDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST, AN_FILE-FSMDefinitionBase::AN_LAST);

  // use file name as name to be displayed in tree
  FSMDefinitionBase::moAttributeValues[ AN_NAME ] = getAttributeValue( AN_FILE );

  // do work of base class
  FSMDefinitionBase::applyAttributes( roInDefinition);
}

// update dom attributes - overwritten of base class
void FSMInclude::updateAttributes( QDomElement& roInOutDefinition ) const
{
  // update attributes in related QDomElement
  FSMDefinitionBase::updateAttributes( roInOutDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST );
}

// method to change id
void FSMInclude::changedId( const QString& roInId )
{
  if ( getAttributeValue( AN_FILE ) == roInId ) return; /// no change --> exit

  // apply change
  setAttributeValue( AN_FILE, roInId );

  // do work of base class
  FSMDefinitionBase::changedId( roInId );

#if 0
  // update id in related dom element
  QDomElement oDomDummy;
  updateAttributes(      oDomDummy );

  // update observers
  emit sigChangedId( *this );
#endif
}



