/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#include <QBrush>
#include <QPen>
#include "port.h"
#include "link.h"
#include "node.h"
#include "fsmelementmanager.h"

const QString Port::gmaAttributeNames[ AN_LAST ] =
{
    "id"       /// AN_ID   element id
    , "number" /// AN_NUM  port number
};

const int Port::miPadding = 1;

Port::Port(int iInNumber,  FSMElementIfc* poInElementParent )
: FSMElementBase( poInElementParent )
, QGraphicsPolygonItem(poInElementParent ? poInElementParent->getGraphicsItem() : 0 )
, moPolygon()
, miNumber( iInNumber )
{
    // create signal slot connections
    if ( poInElementParent )
    {
      // observer change of id of parent element (node)
      connect(
          poInElementParent->getSignalSlotBase(), SIGNAL( sigChangedId( const FSMElementIfc& ))
          , this, SLOT( slotUpdateId( const FSMElementIfc& ) ) );
    }

    setBrush( QBrush( QColor( Qt::black), Qt::NoBrush ) );
    setPen( QPen( QColor( Qt::black), 2 ) );

    moPolygon << QPointF(0, 0) << QPointF(5, 0)
              << QPointF(5, 5)   << QPointF(0, 5)
              << QPointF(0, 0);

    setPolygon( moPolygon);

    setFlag(QGraphicsItem::ItemIsMovable, true);
    setFlag(QGraphicsItem::ItemIsSelectable, true);
    setFlag(QGraphicsItem::ItemSendsGeometryChanges, true);

    // recalculate id
    updateId();

    FSMElementManager::getInstance().addElement( this );
}

Port::~Port()
{
    foreach ( Link* poLink, moLinks )
    {
        moLinks.remove( poLink );

        if ( poLink )
        {
            // release port from link to avoid double delete
            poLink->releasePort( this );
            // free memory and remove reference to link from this object
            delete poLink; poLink= 0;
        }
    }
}

void Port::addLink(Link *poInLink)
{
  if ( !poInLink) return;

  moLinks.insert(poInLink);

  // create connection to inform observers about id change
  connect(
      this,       SIGNAL( sigChangedId( const FSMElementIfc&) )
      , poInLink, SLOT(   slotUpdateId(const FSMElementIfc& ) ) );
}

void Port::removeLink(Link *poInLink)
{
  if ( poInLink )
  {// remove connections to link
    disconnect( this, 0, poInLink, 0 );
  }
  moLinks.remove(poInLink);
}

void Port::updateGeometry()
{
    foreach (Link* poLink, moLinks ) {
        poLink->updateGeometry();
    }
}

void Port::updateId()
{
    Node* poNode = dynamic_cast<Node*>( parentItem() );
    if ( !poNode )
    {
        moId = "";
        return;
    }

    QString oId = poNode->getId();
    oId +="_";
    oId += QString::number( miNumber );

    // apply actions on id change
    changedId( oId );
}


QVariant Port::itemChange(GraphicsItemChange eInChange, const QVariant & roInValue)
{
    if (eInChange == QGraphicsItem::ItemPositionHasChanged) {
        updateGeometry();
    }

    return QGraphicsItem::itemChange( eInChange, roInValue );
}

// create dom element in current working document
QDomElement Port::createDomElement( QDomDocument& /*roInOutDomDocument*/ ) const
{// should not be added to dom document - so return empty element
    return QDomElement::QDomElement();
}

/// update dom attributes
void Port::updateAttributes( QDomElement& roInOutElement ) const
{
    if (roInOutElement.isNull() )
    {
        roInOutElement =
            FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_FSM, const_cast<Port*>( this ) );
    }

    if ( roInOutElement.isNull() ) return;

    roInOutElement.setAttribute( gmaAttributeNames[ AN_ID ], getId()  );
    roInOutElement.setAttribute( gmaAttributeNames[ AN_NUM], miNumber );
}

// slot to connect on signal sigChangedId( const FSMElementIfc&)
void Port::slotUpdateId( const FSMElementIfc& )
{
  // update id and notify observers by emitting signal
  updateId();
}
