/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef FSMELEMENTIFC_H
#define FSMELEMENTIFC_H

#include <QDomDocument>
#include <QDomElement>

/* local */
#include "domviewifc.h"

/*
 forward declarations
 */
class QString;
class QObject;
class QGraphicsItem;
class QTreeWidget;
class QTreeWidgetItem;

class FSMElementIfc: public DomViewIfc
{
public:
    enum ElementType_T
    {
        ET_STATE
        , ET_TRANSITION
        , ET_PORT
        , ET_TRIGGER
        , ET_LAST // end marker
    };

    FSMElementIfc();

    virtual ~FSMElementIfc();

    virtual const QString& getId() const  = 0;
    virtual ElementType_T getType() const = 0;
    // pointer to graphics item
    virtual QGraphicsItem* getGraphicsItem() const = 0;
    // pointer to parent element, if exists
    virtual FSMElementIfc* getElementParent() const = 0;
    // return id of element related to dom parent, if exists - or "" for root nodes
    virtual const QString& getDomParentId() const = 0;
    // create dom element for scene information
    virtual QDomElement createDomSceneElement( QDomDocument& roInOutDoDocuemt ) const = 0;
    // create item to be added to a QTreeWidget
    virtual QTreeWidgetItem* createTreeItem
    (
       const QDomElement& roInDomElement
       , QTreeWidgetItem * poInParentItem = 0
       , QTreeWidget*      poInView       = 0
    ) const = 0;

    // apply attributes
    virtual void applyAttributes( const QDomElement& roInElement ) = 0;
    // assign id of element containing related dom parent
    virtual void setDomParentId( const QString& roInId ) = 0;

private:
    // prevent automatic generation
    FSMElementIfc( const FSMElementIfc&);
    FSMElementIfc& operator=( const FSMElementIfc&);
};

#endif // FSMELEMENTIFC_H
