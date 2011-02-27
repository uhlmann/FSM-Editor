/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef FSMDEFINITIONIFC_H
#define FSMDEFINITIONIFC_H

#include "domviewifc.h"

// forward definitions
class QString;

class FSMDefinitionIfc: public DomViewIfc
{
public:

  enum DefinitionFlag_T
  {
    DF_NONE        = 0x00
    , DF_UNMANAGED = 0x01 // don't register automatically
  };

  enum DefinitionType_T
  {
    DT_ACTION    // action
    , DT_TRIGGER // trigger
    , DT_TIMER   // timer   attributes: name, ms, cnt
    , DT_EVENT   // event
    , DT_MEMBER  // member  attributes: name,type, init
    , DT_INCLUDE // include attributes: file
    , DT_LAST    // end marker
  };

  FSMDefinitionIfc();

  virtual DefinitionType_T getType() const     = 0;
  virtual const QString&   getName() const     = 0;
  virtual const QString&   getTypeName() const = 0;

  // apply attributes
  virtual void applyAttributes( const QDomElement& roInOutElement ) = 0;

  // update dom attributes - to be overwritten in derived classes
  virtual void updateAttributes( QDomElement& roInOutElement ) const = 0;

protected:
    // virtual destructor for derived classes
    virtual ~FSMDefinitionIfc();

    // creation method - clone and assing name
    virtual FSMDefinitionIfc* clone( const QString& roInName ) const = 0;
};

#endif // FSMDEFINITIONIFC_H
