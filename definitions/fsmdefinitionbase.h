/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef FSMDEFINITIONBASE_H
#define FSMDEFINITIONBASE_H

// libraries
#include <QMap>
#include <QObject>
#include <QString>
#include <QStringList>

// local includes
#include "fsmdefinitionifc.h"

class FSMDefinitionBase : public QObject, public FSMDefinitionIfc
{

Q_OBJECT

public:
  // common dom attributes
  enum AttributeName_T
  {
    AN_NAME        /// name attribute
    , AN_LAST      /// end marker
  };


  /*
  create defintion element with type name roInName
  if bInIsPrototype == true, element should be used as prototype
  for cloning
*/
  FSMDefinitionBase
      (
          const QString&     roInName
          , const QString&   roInTypeName
          , DefinitionType_T eInType
          , bool             bInIsPrototype = false
          , DefinitionFlag_T eInFlags       = DF_NONE
      );

  // create dom element in current working document
  QDomElement createDomElement( QDomDocument& roInOutDomDocument ) const;

  // create an element related to a tag name
  static FSMDefinitionIfc* createByName( const QString& roInTypeName, const QString& roInName );

  // returns string list with definition names
  static const QStringList getDefinitionNames( void );

  inline DefinitionType_T getType() const { return meType; }
  inline const QString&   getName() const { return moAttributeValues[ AN_NAME ]; }
  inline const QString&   getTypeName() const { return moTypeName; }
  // return attribute name related to AN_ with number uiIdx < AN_LAST
  virtual QString      getAttributeName( unsigned int uiIdx ) const;
  virtual unsigned int getAttributeCount() const;
  // access to signal slot functionality
  inline  QObject* getSignalSlotBase() const { return const_cast<QObject*>( dynamic_cast<const QObject*>( this ) ); }
  // access to rleated dom element
  QDomElement getDomElement() const;
  // assign attribute - returning false if not assigned
  bool setAttribute( const QString& roInName, const QString& roInValue);


protected:
  typedef QMap<QString, FSMDefinitionBase*> TagNameToDefinitionMap_T;

  /*
   ATTRIBUTES
  */
  // map with prototypes (not allocated with new!)
  static TagNameToDefinitionMap_T& getPrototypes();
  // name of common attributes
  static const QString moAttributeNames[ AN_LAST ];

  // flags
  DefinitionFlag_T meFlags; // definition flags
  // attribute values as strings
  QString          moAttributeValues[ AN_LAST ];
  // typename/tag of definition
  QString          moTypeName;
  // type as enum
  DefinitionType_T meType;

  // virtual destructor
  virtual ~FSMDefinitionBase();

  // creation method
  virtual FSMDefinitionBase* clone( const QString& /*roInName*/ ) const { return 0; }
  // apply attributes
  void applyAttributes( const QDomElement& roInElement );
  // apply attributes and assign values in an arry
  void applyAttributes
  (
    const QDomElement& roInDefinition
    , const QString*   paoInAttributeNames
    , QString*         paoInOutAttributeValues
    , unsigned int     uiInSize
    , unsigned int     uiInIdxId = 0 // element when changedId shall be called
  );
  // update attributes in related QDomElement
  virtual void updateAttributes
  (
      QDomElement&    roInOutDefinition
      , const QString* paoInAttributeNames
      , const QString* paoroInAttributeValues
      , unsigned int  uiInArraySize
  ) const;

  // update dom attributes - to be overwritten in derived classes
  virtual void updateAttributes( QDomElement& roInOutDefinition ) const;
  // method to change id - may be specialized for ids different
  // from moAttributes[ AN_NAME ]
  virtual void changedId( const QString& roInId );

signals:
    // signal to inform about id change
    void sigChangedId( const FSMDefinitionIfc& );

};

#endif // FSMDEFINITIONBASE_H
