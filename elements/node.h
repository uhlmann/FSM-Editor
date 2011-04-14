/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef NODE_H
#define NODE_H

#include <QApplication>
#include <QColor>
#include <QGraphicsItem>
#include <QTreeWidgetItem>


#include "fsmelementbase.h"

class Port;
class QDomElement;
class QGraphicsRectItem;
class QTreeWidget;

class Node : public FSMElementBase, public QGraphicsItem
{
    Q_DECLARE_TR_FUNCTIONS(Node)

public:
    enum AttributeName_T
    {
        AN_ID
        , AN_NAME        /// name of node
        , AN_TYPE        /// type
        , AN_ENTER       /// enter action
        , AN_EXIT        /// exit action
        , AN_TSTARTENTER /// start timer when state is entered
        , AN_TSTARTEXIT  /// start timer when state is exited
        , AN_TSTOPENTER  /// stop timer when state is entered
        , AN_TSTOPEXIT   /// stop timer when state is exited
        , AN_EVENTENTER  /// event sent when state is entered
        , AN_EVENTEXIT   /// event sent when state is exited
        , AN_X           /// x coordinate
        , AN_Y           /// y coordinate
        , AN_WIDTH       /// width
        , AN_HEIGHT      /// heigth
        , AN_LAST        /// end marker
    };

    // pointer to dom element containing node information
    // roInDefaultName - name if poInDomElement is 0
    Node( const QDomElement* poInDomElement, const QString& roInDefaultName, bool bInIsPrototype = false );
    ~Node();

    /* changing attributes */
    void setId( const QString& roInId );
    // set Name
    void setName(const QString & roInText );
    inline QString name() const { return moId; }
    void setTextColor(const QColor & roInColor);
    QColor textColor() const;
    void setOutlineColor(const QColor &roInColor);
    QColor outlineColor() const;
    void setBackgroundColor(const QColor &roInolor);
    QColor backgroundColor() const;

    /* overwritten methods of base class */
    QRectF boundingRect() const;
    QPainterPath shape() const;
    // creates a port for connection/link to poToNode
    Port* createConnectionPort( const Node* poToNode );
    // create item to be added to a QTreeWidget or returns 0 if not supported by object
    QTreeWidgetItem* createTreeItem
    (
       const QDomElement& roInDomElement
       , QTreeWidgetItem *poInParentItem = 0
       , QTreeWidget*       poInView       = 0
    ) const;

    void paint(QPainter *painter,
               const QStyleOptionGraphicsItem *option, QWidget *widget);

    /* interface FSMELementIfc */
    inline ElementType_T getType() const { return ET_STATE; }
    // pointer to graphics item
    inline QGraphicsItem* getGraphicsItem() const
    { return const_cast<Node*>(this); }
    // update dom attributes
    void updateAttributes( QDomElement& roInOutElement ) const;
    // update scene attributes
    void updateAttributesScene( QDomElement& roInOutElement) const;
    // assign attribute - returning false if not assigned
    bool setAttribute( const QString& roInName, const QString& roInValue);
    // apply attributes
    void applyAttributes( const QDomElement& roInElement );

#if 0
    // calculate id by Element class using related Dom Element
    static QString calcId( const QDomElement& roInElement );
#endif

    static const QString gmaAttributeNames[ AN_LAST ];

protected:
    // create selection handles
    void createSelectionHandles();

    // update handle positions
    void updateHandlePositions();

    // show/hide handles
    void setSelectionHandlesVisible( bool bInShow );

    // calculate id from a dom element
    QString calculateId( const QDomElement& roInElement) const;


    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    QVariant itemChange(GraphicsItemChange change,
                        const QVariant &value);

    int                  miPortCnt; // ports counter
    qreal                mdPadding;

    // attribute values
    QString moType;
    QString moEnterActions;
    QString moExitActions;
    QString moEnterStartTimers;
    QString moExitStartTimers;
    QString moEnterStopTimers;
    QString moExitStopTimers;
    QString moEnterEvent;
    QString moExitEvent;

    // selection handle
    QGraphicsRectItem* mpoSelectionHandleTopLeft;
    QGraphicsRectItem* mpoSelectionHandleTopRight;
    QGraphicsRectItem* mpoSelectionHandleBottomLeft;
    QGraphicsRectItem* mpoSelectionHandleBottomRight;


    QColor moTextColor;
    QColor moBackgroundColor;
    QColor moOutlineColor;

    static const qreal   gmdLimitMaxWidth; /// maximum text width


private:
    QRectF outlineRect() const;
    int roundness(double size) const;
};

#endif
