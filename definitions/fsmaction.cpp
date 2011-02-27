/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include <QString>
#include "fsmaction.h"

using namespace FSMDefinition;

/// create and register prototype
static FSMAction goActionPrototype( "proto_action", true );

// name of common attributes
const QString FSMAction::moAttributeNames[ FSMAction::AN_LAST-FSMDefinitionBase::AN_LAST ] =
{
  QString( "param") // AN_PARAM
};

FSMAction::FSMAction(const QString& roInName, bool bInIsPrototype )
  : FSMDefinitionBase( roInName,"action", DT_ACTION, bInIsPrototype )
{
  // clear Strings
  unsigned int uiIdx = 0;
  for ( uiIdx = 0; uiIdx < AN_LAST-FSMDefinitionBase::AN_LAST; ++uiIdx)
  {
    moAttributeValues[ uiIdx ] = "";
  }
}

// creation method
FSMAction* FSMAction::clone( const QString& roInName ) const
{
  return new FSMAction( roInName );
}

/* overwritten of base class */
// return attribute name related to AN_ with number uiIdx < AN_LAST
QString      FSMAction::getAttributeName( unsigned int uiIdx ) const
{
  if ( uiIdx >= AN_LAST ) return QString();
  if ( uiIdx >= FSMDefinitionBase::AN_LAST )
  {
    return moAttributeNames[ uiIdx-FSMDefinitionBase::AN_LAST];
  }

  return FSMDefinitionBase::getAttributeName( uiIdx );
}

// apply attributes
void FSMAction::applyAttributes( const QDomElement& roInDefinition )
{
  // do work of base class
  FSMDefinitionBase::applyAttributes( roInDefinition);

  // handle parameters of this class
  FSMDefinitionBase::applyAttributes( roInDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST, AN_LAST);
}

// update dom attributes - to be overwritten in derived classes
void FSMAction::updateAttributes( QDomElement& roInOutDefinition ) const
{
  // do common work
  FSMDefinitionBase::updateAttributes( roInOutDefinition );

  // update attributes in related QDomElement
  FSMDefinitionBase::updateAttributes( roInOutDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST );
}
