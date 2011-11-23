/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef LINK_H
#define LINK_H

#include <QString>
#include <QGraphicsPathItem>
#include "fsmelementbase.h"

class Node;
class QDomElement;
class Port;

class Link : public FSMElementBase, public QGraphicsPathItem
{
public:
    enum AttributeName_T
    {
        AN_ID
        , AN_TRIGGER   /// trigger for transition
        , AN_BEGIN     /// start state
        , AN_END       /// destination state
        , AN_ACTION    /// action to be performed on transition
        , AN_EVENT     /// event sent when transition is performed
        , AN_CONDITION /// condition for transition
        , AN_POLYGON   /// list of point coordinates separated by spaces
        , AN_LAST      /// end marker
    };

    Link(Node *fromNode, Node *toNode, bool bInDirected=true, bool bInPrototype = false );
    Link( const QDomElement * poInElement, bool bInDirected=true, bool bInPrototype = false  );
    ~Link();
    // release Port and set member pointer to 0 - no delete
    // true if mpoFromPort or mpoToPort matches
    bool releasePort( Port* poInPort);

    Port *fromPort() const;
    Port *toPort() const;

    void setColor(const QColor &color);
    QColor color() const;

    void updateGeometry();
    // recalculate id
    void updateId();
    // update start and end state names
    void updateStates();

    /* interface FSMELementIfc */
    const QString& getId() const;
    inline ElementType_T getType() const { return ET_TRANSITION; }
    // pointer to graphics item
    inline QGraphicsItem* getGraphicsItem() const
    { return const_cast<Link*>(this); }
    // update dom attributes
    void updateAttributes( QDomElement& roInOutElement ) const;
    // update scene attributes
    void updateAttributesScene( QDomElement& roInOutElement) const;
    // assign attribute - returning false if not assigned
    bool setAttribute( const QString& roInName, const QString& roInValue);
    // apply attributes
    void applyAttributes( const QDomElement& roInElement );
    void applySceneAttributes( const QDomElement& roInElement );

protected:
    // calculate id from a dom element
    QString calculateId( const QDomElement& roInElement) const;

    QRectF boundingRect() const;
    void mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event );
    void mouseMoveEvent ( QGraphicsSceneMouseEvent * event );

    static const QString gmaAttributeNames[ AN_LAST ];

    QGraphicsSimpleTextItem* mpoTextItem; /// fst line: trigger + cond, snd line:action
    QString moTrigger;
    QString moStateFrom;
    QString moStateTo;
    QString moAction;
    QString moEvent;
    QString moCondition;

    void paint
    (
        QPainter *painter
        , const QStyleOptionGraphicsItem *option,
        QWidget *widget = 0);

    // assign new text to text child item
    void updateText();

private:
    Port    *mpoFromPort;
    Port    *mpoToPort;

    bool mbDrawArrow;



    static int gmiHelperCnt;
    QPointF moControlPoint;

signals:

public slots:
    // slot to connect on signal sigChangedId( const FSMElementIfc&)
    void slotUpdateId( const FSMElementIfc& );
};

#endif
