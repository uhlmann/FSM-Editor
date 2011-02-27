/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef PORT_H
#define PORT_H

#include <QGraphicsPolygonItem>
#include <QSet>

#include "fsmelementbase.h"


QT_BEGIN_NAMESPACE
class QPolygonF;
QT_END_NAMESPACE

/*
forward declarations
*/
class Link;

/*
 state representation
 - name (key)
 - link to next level
 - enter action(s)
 - timer to start
 - exit action(s)

 */
class Port : public FSMElementBase, public QGraphicsPolygonItem
{
public:
    enum AttributeName_T
    {
        AN_ID     /// attribute id
        , AN_NUM  /// number of port
        , AN_LAST /// end marker
    };

    explicit Port( int iInNumber,  FSMElementIfc* poInElementParent );

    virtual ~Port();

    inline QPolygonF polygon() const { return moPolygon; }

    void addLink(Link *poInLink);
    void removeLink(Link *poInLink);
    inline const QSet<Link*>& links() const { return moLinks; }

    void updateGeometry();
    void updateId();

    /* interface FSMELementIfc */
    inline ElementType_T getType() const { return ET_PORT; }
    // pointer to graphics item
    inline QGraphicsItem* getGraphicsItem() const
    { return const_cast<Port*>(this); }
    // create dom element in current working document
    QDomElement createDomElement( QDomDocument& roInOutDomDocument ) const;
    /// update dom attributes
    void updateAttributes( QDomElement& roInOutElement ) const;
    /// update Text widget

protected:

    // item has has changed
    QVariant itemChange(GraphicsItemChange change, const QVariant &value);

    static const QString gmaAttributeNames[ AN_LAST ];

    static const int miPadding;

    QPolygonF moPolygon;

    QSet<Link *> moLinks;

    int          miNumber; /// port number

signals:

public slots:
    // slot to connect on signal sigChangedId( const FSMElementIfc&)
    void slotUpdateId( const FSMElementIfc& );
};

#endif // PORT_H
