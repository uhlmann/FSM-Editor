/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include "fsmevent.h"

using namespace FSMDefinition;

static FSMEvent goEventPrototpye( "proto_event", true );

FSMEvent::FSMEvent( const QString& roInName, bool bInIsPrototype)
  : FSMDefinitionBase( roInName, "event", DT_EVENT, bInIsPrototype )
{
}

// creation method
FSMEvent* FSMEvent::clone( const QString& roInName ) const
{
  return new FSMEvent( roInName );
}
