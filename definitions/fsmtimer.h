/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#ifndef TIMER_H
#define TIMER_H

#include "fsmdefinitionbase.h"

namespace FSMDefinition
{
  class FSMTimer : public FSMDefinitionBase
  {
  public:
    // attributes
    // common dom attributes
    enum AttributeName_T
    {
      AN_MS = FSMDefinitionBase::AN_LAST   /// ms attribute
      , AN_CNT                             /// cnt attribute
      , AN_LAST                            /// end marker
    };

    // name of common attributes
    static const QString moAttributeNames[ AN_LAST-FSMDefinitionBase::AN_LAST ];


    FSMTimer( const QString& roInName, bool bInIsPrototype=false );

    virtual ~FSMTimer() {}

    // creation method
    FSMTimer* clone( const QString& roInName ) const;

    /* overwritten of base class */
    // return attribute name related to AN_ with number uiIdx < AN_LAST
    QString      getAttributeName( unsigned int uiIdx ) const;
    inline unsigned int getAttributeCount()             const { return AN_LAST; }

    // apply attributes
    void applyAttributes( const QDomElement& roInElement );
    // update dom attributes - to be overwritten in derived classes
    void updateAttributes( QDomElement& roInOutElement ) const;

  protected:
    // attribute values as strings
    QString          moAttributeValues[ AN_LAST-FSMDefinitionBase::AN_LAST ];
  };
}

#endif // TIMER_H
