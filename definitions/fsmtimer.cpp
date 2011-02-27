/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include "fsmtimer.h"

using namespace FSMDefinition;

static FSMTimer goTimerPrototype( "proto_timer", true );

// name of common attributes
const QString FSMTimer::moAttributeNames[ FSMTimer::AN_LAST-FSMDefinitionBase::AN_LAST ] =
{
  QString( "ms")    // AN_MS
, QString( "cnt")   // AN_CNT
};

FSMTimer::FSMTimer( const QString& roInName, bool bInIsPrototype)
  : FSMDefinitionBase( roInName, "timer", DT_TIMER, bInIsPrototype )
  , moMSecs()
  , moCnt()
{
}

// creation method
FSMTimer* FSMTimer::clone( const QString& roInName ) const
{
  return new FSMTimer( roInName);
}

/* overwritten of base class */
// return attribute name related to AN_ with number uiIdx < AN_LAST
QString  FSMTimer::getAttributeName( unsigned int uiIdx ) const
{
  if ( uiIdx >= AN_LAST ) return QString();
  if ( uiIdx >= FSMDefinitionBase::AN_LAST )
  {
    return moAttributeNames[ uiIdx-FSMDefinitionBase::AN_LAST];
  }

  return FSMDefinitionBase::getAttributeName( uiIdx );
}

// apply attributes
void FSMTimer::applyAttributes( const QDomElement& roInDefinition )
{
  // do work of base class
  FSMDefinitionBase::applyAttributes( roInDefinition);

  // handle parameters of this class
  FSMDefinitionBase::applyAttributes( roInDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST, AN_LAST);

}

// update dom attributes - to be overwritten in derived classes
void FSMTimer::updateAttributes( QDomElement& roInOutDefinition ) const
{
  // do common work
  FSMDefinitionBase::updateAttributes( roInOutDefinition );

  // update attributes in related QDomElement
  FSMDefinitionBase::updateAttributes( roInOutDefinition, moAttributeNames, moAttributeValues, AN_LAST-FSMDefinitionBase::AN_LAST );
}
