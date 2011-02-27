/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#ifndef TRIGGER_H
#define TRIGGER_H

#include "fsmdefinitionbase.h"

namespace FSMDefinition
{
  class FSMTrigger : public FSMDefinitionBase
  {
  public:
    // attributes
    // common dom attributes
    enum AttributeName_T
    {
      AN_PARAM = FSMDefinitionBase::AN_LAST   /// name attribute
      , AN_LAST                               /// end marker
    };

    // name of defintion type
    QString          moParam;

    // name of common attributes
    static const QString moAttributeNames[ AN_LAST-FSMDefinitionBase::AN_LAST ];


    FSMTrigger( const QString& roInName, bool bInIsPrototype=false );
    virtual ~FSMTrigger() {}

    // creation method
    FSMTrigger* clone( const QString& roInName ) const;

    /* overwritten of base class */
    // return attribute name related to AN_ with number uiIdx < AN_LAST
    QString      getAttributeName( unsigned int uiIdx ) const;
    inline unsigned int getAttributeCount()             const { return AN_LAST; }

    // apply attributes
    void applyAttributes( const QDomElement& roInElement );
    // update dom attributes - to be overwritten in derived classes
    void updateAttributes( QDomElement& roInOutElement ) const;
  };
}

#endif // TRIGGER_H
