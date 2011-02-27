/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#ifndef EVENT_H
#define EVENT_H

#include "fsmdefinitionbase.h"

namespace FSMDefinition
{
  class FSMEvent : public FSMDefinitionBase
  {
  public:
    FSMEvent( const QString& roInName, bool bInIsPrototype=false );

    virtual ~FSMEvent() {}

    // creation method
    FSMEvent* clone( const QString& roInName ) const;

    /* overwritten of base class */
    inline unsigned int getAttributeCount()             const { return AN_LAST; }
  };
}

#endif // EVENT_H
