/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#ifndef MEMBER_H
#define MEMBER_H

#include "fsmdefinitionbase.h"

namespace FSMDefinition
{
  class FSMMember : public FSMDefinitionBase
  {
  public:
    // attributes
    // common dom attributes
    enum AttributeName_T
    {
      AN_TYPE = FSMDefinitionBase::AN_LAST   /// type attribute
      , AN_INIT                              /// attribute init
      , AN_LAST                              /// end marker
    };

    // name of defintion type
    QString          moType; /// data type of member
    QString          moInit; /// inital value

    // name of common attributes
    static const QString moAttributeNames[ AN_LAST-FSMDefinitionBase::AN_LAST ];

    FSMMember( const QString& roInName, bool bInIsPrototype=false);

    virtual ~FSMMember() {}

    // creation method
    FSMMember* clone( const QString& roInName ) const;

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

#endif // MEMBER_H
