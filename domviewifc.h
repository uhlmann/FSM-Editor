/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef DOMVIEWIFC_H
#define DOMVIEWIFC_H

#include <QDomElement>


class QObject;
class QDomDocument;

class DomViewIfc
{
public:
  DomViewIfc() {};

  // create dom element in current working document
  virtual QDomElement createDomElement( QDomDocument& roInOutDomDocument ) const  = 0;
  // access to signal slot functionality
  virtual QObject* getSignalSlotBase() const = 0;
  // access to rleated dom element
  virtual QDomElement getDomElement() const = 0;
  // assign attribute - returning false if not assigned
  virtual bool setAttribute( const QString& roInName, const QString& roInValue) = 0;


protected:
  virtual ~DomViewIfc() {};


};

#endif // DOMVIEWIFC_H
