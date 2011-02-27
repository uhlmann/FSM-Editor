/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#ifndef INCLUDE_H
#define INCLUDE_H


#include "fsmdefinitionbase.h"

namespace FSMDefinition
{
  class FSMInclude : public FSMDefinitionBase
  {

  Q_OBJECT

  public:
    // common dom attributes
    enum AttributeName_T
    {
      AN_FILE   = FSMDefinitionBase::AN_LAST /// file attribute
      , AN_LAST                              /// end marker
    };

    // name of defintion type
    QString          moFile;

    // name of common attributes
    static const QString moAttributeNames[ AN_LAST ];

    FSMInclude( const QString& roInName, bool bInIsPrototype=false );

    virtual ~FSMInclude() {}

    // creation method
    FSMInclude* clone( const QString& roInName ) const;

    /* overwritten of base class */
    // return attribute name related to AN_ with number uiIdx < AN_LAST
    QString             getAttributeName( unsigned int uiIdx ) const;
    QString             getAttributeValue( unsigned int uiIdx ) const;
    bool                setAttributeValue( unsigned int uiIdx, const QString& );
    // apply attributes
    void applyAttributes( const QDomElement& roInElement );
    // update dom attributes - to be overwritten in derived classes
    void updateAttributes( QDomElement& roInOutElement ) const;

  protected:
    // attribute values as strings
    QString              moAttributeValues[ AN_LAST-FSMDefinitionBase::AN_LAST  ];

    // return index of element which acts as id
    inline unsigned int getIdIdx() const { return AN_FILE; }
    // method to change id - may be specialized for ids different
    // from moAttributes[ AN_NAME ]
    virtual void changedId( const QString& roInId );

  signals:
      // signal to inform about id change
      void sigChangedId( const FSMDefinitionIfc& );
  };
}

#endif // INCLUDE_H
