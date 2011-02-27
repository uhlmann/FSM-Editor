/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include "fsmmember.h"

using namespace FSMDefinition;

// register prototype
static FSMMember goMemberPrototype( "proto_member", true );

// name of common attributes
const QString FSMMember::moAttributeNames[ FSMMember::AN_LAST-FSMDefinitionBase::AN_LAST ] =
{
  QString( "type") // AN_TYPE
, QString( "init") // AN_INIT
};

FSMMember::FSMMember( const QString& roInName, bool bInIsPrototype)
  : FSMDefinitionBase( roInName, "member", DT_MEMBER, bInIsPrototype )
, moType()
, moInit()
{
}

// creation method
FSMMember* FSMMember::clone( const QString& roInName ) const
{
  return new FSMMember( roInName );
}

/* overwritten of base class */
// return attribute name related to AN_ with number uiIdx < AN_LAST
QString      FSMMember::getAttributeName( unsigned int uiIdx ) const
{
  if ( uiIdx >= AN_LAST ) return QString();
  if ( uiIdx >= FSMDefinitionBase::AN_LAST )
  {
    return moAttributeNames[ uiIdx-FSMDefinitionBase::AN_LAST];
  }

  return FSMDefinitionBase::getAttributeName( uiIdx );
}

// apply attributes
void FSMMember::applyAttributes( const QDomElement& roInDefinition )
{
  // do work of base class
  FSMDefinitionBase::applyAttributes( roInDefinition);

  // handle parameters of this class
  FSMDefinitionBase::applyAttributes( roInDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST, AN_LAST);
}

// update dom attributes - to be overwritten in derived classes
void FSMMember::updateAttributes( QDomElement& roInOutDefinition ) const
{
  // do common work
  FSMDefinitionBase::updateAttributes( roInOutDefinition );

  // update attributes in related QDomElement
  FSMDefinitionBase::updateAttributes( roInOutDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST );
}
