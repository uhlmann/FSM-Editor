/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include "fsmtrigger.h"

using namespace FSMDefinition;

// register prototype
static FSMTrigger goTriggerPrototype( "proto_trigger", true );// name of common attributes
// additional attributes
const QString FSMTrigger::moAttributeNames[ FSMTrigger::AN_LAST-FSMDefinitionBase::AN_LAST ] =
{
  QString( "param") // AN_PARAM
};


FSMTrigger::FSMTrigger( const QString& roInName, bool bInIsPrototype )
  : FSMDefinitionBase( roInName, "trigger", DT_TRIGGER, bInIsPrototype )
, moParam()
{
  unsigned int uiIdx = 0;
  for ( uiIdx = 0; uiIdx < AN_LAST-FSMDefinitionBase::AN_LAST; ++uiIdx)
  {
    moAttributeValues[ uiIdx ] = "";
  }
}

// creation method
FSMTrigger* FSMTrigger::clone( const QString& roInName ) const
{
  return new FSMTrigger( roInName );
}

/* overwritten of base class */
// return attribute name related to AN_ with number uiIdx < AN_LAST
QString FSMTrigger::getAttributeName( unsigned int uiIdx ) const
{
  if ( uiIdx >= AN_LAST ) return QString();
  if ( uiIdx >= FSMDefinitionBase::AN_LAST )
  {
    return moAttributeNames[ uiIdx-FSMDefinitionBase::AN_LAST];
  }

  return FSMDefinitionBase::getAttributeName( uiIdx );
}

// apply attributes
void FSMTrigger::applyAttributes( const QDomElement& roInDefinition )
{
  // handle parameters of this class
  FSMDefinitionBase::applyAttributes( roInDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST, AN_LAST);

  // do work of base class
  FSMDefinitionBase::applyAttributes( roInDefinition);

}

// update dom attributes - to be overwritten in derived classes
void FSMTrigger::updateAttributes( QDomElement& roInOutDefinition ) const
{
  // do common work
  FSMDefinitionBase::updateAttributes( roInOutDefinition );

  // update attributes in related QDomElement
  FSMDefinitionBase::updateAttributes( roInOutDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST );
}
