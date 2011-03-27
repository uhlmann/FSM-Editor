/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#include <QtGui>
#include <QDomElement>

#include "node.h"
#include "port.h"
#include "fsmelementmanager.h"

const qreal   Node::gmdLimitMaxWidth             = 250.0;
const QString Node::gmaAttributeNames[ AN_LAST ] =
{
  "id"                /// AN_ID   element id
      , "name"        /// AN_NAME name of node
      , "type"        /// AN_TYPE type == entry to mark enter state of level
      , "enter"       /// AN_ENTER enter action(s)
      , "exit"        /// AN_EXIT   exit action
      , "tstartenter" /// AN_TSTARTENTER start timers when state is entered
      , "tstartexit"  /// AN_TSTARTEXIT  start timer when state is exited
      , "tstopenter"  /// AN_TSTOPENTER  stop timer when state is entered
      , "tstopexit"   /// AN_TSTOPEXIT   stop timer when state is exited
      , "evententer"
      , "eventexit"
      , "x"           /// AN_X      x coordinate
      , "y"           /// AN_Y      y coordinate
      , "width"       /// AN_WIDTH  width
      , "height"      /// AN_HEIGHT heigth
    };

static Node goNodeProto(0, "node_prototype", true );

// pointer to dom element containing node information
// roInDefaultName - name if poInDomElement is 0
Node::Node( const QDomElement* poInDomElement, const QString& roInDefaultName, bool bInIsPrototype )
  : FSMElementBase( 0,0, gmaFSMElementTagName[ ET_STATE], bInIsPrototype )
  , miPortCnt( 0 )
  , mdPadding( 8.0 )
  , moType( "" )
  , moEnterActions()
  , moExitActions()
  , moEnterStartTimers()
  , moExitStartTimers()
  , moEnterStopTimers()
  , moExitStopTimers()
  , moEnterEvent()
  , moExitEvent()
{
  if (bInIsPrototype ) return; // no further registration

  moId = roInDefaultName;

  if ( poInDomElement )
  {
    applyAttributes( *poInDomElement );
  }
  moTextColor = Qt::darkGreen;
  moOutlineColor = Qt::darkBlue;
  moBackgroundColor = Qt::white;

  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges );

  FSMElementManager::getInstance().addElement( this );
}

Node::~Node()
{
}

void Node::setId( const QString& roInId )
{
  if ( moId == roInId ) return;

  // apply changes of id
  changedId( roInId );
}

void Node::setName(const QString & roInText )
{
  prepareGeometryChange();
  setId( roInText );

  // update scene
  update();
}

void Node::setTextColor(const QColor &roInColor)
{
  moTextColor = roInColor;
  update();
}

QColor Node::textColor() const
{
  return moTextColor;
}

void Node::setOutlineColor(const QColor &color)
{
  moOutlineColor = color;
  update();
}

QColor Node::outlineColor() const
{
  return moOutlineColor;
}

void Node::setBackgroundColor(const QColor &roInColor)
{
  moBackgroundColor = roInColor;
  update();
}

QColor Node::backgroundColor() const
{
  return moBackgroundColor;
}

QRectF Node::boundingRect() const
{
  const int Margin = 1;
  return outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
}

QPainterPath Node::shape() const
{
  QRectF rect = outlineRect();

  QPainterPath path;
  path.addRoundRect(rect, roundness(rect.width()),
                    roundness(rect.height()));
  return path;
}

// creates a port for connection/link to poToNode
Port* Node::createConnectionPort( const Node* poToNode )
{
  if ( !poToNode ) return 0;

  // create new port
  Port* poPort = new Port( miPortCnt++, this );

  // calculate position of the port
  QLineF oLine(pos(), poToNode->pos());
  QPolygonF oPolygon = shape().toFillPolygon();
  QPointF p1 = oPolygon.first() + pos();
  QPointF p2;
  QPointF oIntersectionPoint;
  QLineF oSegment;
  for (int i = 1; i < oPolygon.count(); ++i)
  {
    p2 = oPolygon.at(i) + pos();
    oSegment = QLineF(p1, p2);
    QLineF::IntersectType oIntersectType =
        oSegment.intersect(oLine, &oIntersectionPoint);
    if (oIntersectType == QLineF::BoundedIntersection)
      break;
    p1 = p2;
  }

  /// map to local coordinates and assign to port position
  poPort->setPos( mapFromScene(oIntersectionPoint) - poPort->boundingRect().center() );

  return poPort;
}

// create item to be added to a QTreeWidget or returns 0 if not supported by object
QTreeWidgetItem* Node::createTreeItem
    (
        const QDomElement& /*roInDomElement*/
        , QTreeWidgetItem* poInParentItem
        , QTreeWidget*     poInView
        ) const
{
  if ( poInParentItem )
  {
    return new QTreeWidgetItem( poInParentItem );
  }
  else if ( poInView != 0 )
  {
    return new QTreeWidgetItem( poInView );
  }

  return 0;
}


void Node::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget * /* widget */)
{
  QPen pen(moOutlineColor);
  if (option->state & QStyle::State_Selected) {
    pen.setStyle(Qt::DotLine);
    pen.setWidth(2);
  }
  painter->setPen(pen);
  painter->setBrush(moBackgroundColor);

  QRectF oRect = outlineRect();
  painter->drawRoundRect(oRect, roundness(oRect.width()),
                         roundness(oRect.height()));

  painter->setPen(moTextColor);
  QString oText = moId;
  if ( !moEnterActions.isEmpty() )
  {
    oText += "\n";
    oText += moEnterActions;
  }
  if ( !moEnterStartTimers.isEmpty() )
  {
    oText += "\n";
    oText += moEnterStartTimers;
  }
  if ( !moExitStartTimers.isEmpty() )
  {
    oText += "\n";
    oText += moExitStartTimers;
  }
  if ( !moEnterStopTimers.isEmpty() )
  {
    oText += "\n";
    oText += moEnterStopTimers;
  }
  if ( !moExitStopTimers.isEmpty() )
  {
    oText += "\n";
    oText += moExitStopTimers;
  }
  if ( !moExitActions.isEmpty() )
  {
    oText += "\n";
    oText += moExitActions;
  }

  painter->drawText(oRect, Qt::AlignCenter, oText );
}

void Node::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
  QString oNodeName = QInputDialog::getText(event->widget(),
                                       tr("Edit Text"), tr("Enter new text:"),
                                       QLineEdit::Normal, name() );

  if (!oNodeName.isEmpty())
    setName(oNodeName);
}

QVariant Node::itemChange(GraphicsItemChange eInChange,
                          const QVariant &roInValue)
{
  if (eInChange == ItemPositionHasChanged) {
    foreach (QGraphicsItem *poChild, childItems() )
    {
      Port* poPort = dynamic_cast<Port*>( poChild);
      if ( poPort )
      {
        poPort->updateGeometry();
      }
    }
    // update scene attributes
    QDomElement oDomDummy;
    updateAttributesScene( oDomDummy );
  }

  // do work of base class
  return QGraphicsItem::itemChange(eInChange, roInValue);
}

QRectF Node::outlineRect() const
{
  enum TextType_T
  {
    TT_NAME
        , TT_ENTERACT
        , TT_TIMER
        , TT_EXITACT
        , TT_LAST
      };

  QRectF oRectArr[ TT_LAST ];

  QFontMetricsF metrics = qApp->font();
  oRectArr[ TT_NAME ]     = metrics.boundingRect(moId);
  oRectArr[ TT_ENTERACT ] = metrics.boundingRect( moEnterActions );
  oRectArr[ TT_TIMER ]    = metrics.boundingRect( moEnterStartTimers  );
  oRectArr[ TT_EXITACT ]  = metrics.boundingRect( moExitActions  );

  unsigned int uiIdx = 0;
  qreal dWidthMax = 0;
  for ( uiIdx = TT_NAME; uiIdx < TT_LAST; ++uiIdx )
  {
    if ( oRectArr[ uiIdx ].width() > dWidthMax ) dWidthMax = oRectArr[ uiIdx ].width();
  }
  if ( dWidthMax > gmdLimitMaxWidth )
  {
    dWidthMax = gmdLimitMaxWidth;
  }

  QRectF oRectText = oRectArr[ TT_NAME ];

  oRectText.setWidth( dWidthMax);
  oRectText.setHeight(
      3*mdPadding/2 + oRectArr[ TT_ENTERACT ].height()
      + oRectArr[ TT_TIMER ].height() + oRectArr[ TT_EXITACT ].height() );

  // add padding round the text items
  oRectText.adjust(-mdPadding, -mdPadding, +mdPadding, +mdPadding);
  oRectText.translate(-oRectText.center());
  return oRectText;
}

int Node::roundness(double size) const
{
  const int Diameter = 12;
  return 100 * Diameter / int(size);
}

/// update dom attributes
void Node::updateAttributes( QDomElement& roInOutElement ) const
{
  if (roInOutElement.isNull() )
  {
    roInOutElement =
        FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_FSM, const_cast<Node*>( this ) );
  }

  if ( roInOutElement.isNull() ) return;

  roInOutElement.setAttribute( gmaAttributeNames[ AN_ID   ], moId );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_NAME ], moId );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_TYPE ], moType  );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_ENTER ], moEnterActions );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_EXIT ], moExitActions );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_TSTARTENTER ], moEnterStartTimers );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_TSTARTEXIT ], moExitStartTimers );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_TSTOPENTER ], moEnterStopTimers );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_TSTOPEXIT ], moExitStopTimers );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_EVENTENTER ], moEnterEvent );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_EVENTEXIT ], moExitEvent );



}

// assign new scene attributes apply derived methods
void Node::updateAttributesScene( QDomElement& roInOutElement) const
{
  if (roInOutElement.isNull() )
  {
    roInOutElement =
        FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_SCENE, const_cast<Node*>( this ) );
  }

  if ( roInOutElement.isNull() ) return;

  roInOutElement.setAttribute( gmaAttributeNames[ AN_ID   ], getId() );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_X], pos().x());
  roInOutElement.setAttribute( gmaAttributeNames[ AN_Y], pos().y());
  roInOutElement.setAttribute( gmaAttributeNames[ AN_WIDTH], boundingRect().width());
  roInOutElement.setAttribute( gmaAttributeNames[ AN_HEIGHT], boundingRect().height());
}

// assign attribute - returning false if not assigned
bool Node::setAttribute( const QString& roInName, const QString& roInValue)
{
  bool bResult = false;
  if ( roInName == gmaAttributeNames[ AN_ID] )
  {// special actions for change of id
  }
  else if ( roInName == gmaAttributeNames[ AN_NAME ])
  {// special actions for change of id

  }
  else
  {
    // common handling in base class
    bResult = FSMElementBase::setAttribute( roInName, roInValue);
  }

  return bResult;
}

// apply attributes
void Node::applyAttributes( const QDomElement& roInElement )
{
  QString oAttr = roInElement.attribute( gmaAttributeNames[ AN_NAME ], "" );
  if (!oAttr.isEmpty()) moId = oAttr;
  oAttr = roInElement.attribute( gmaAttributeNames[ AN_ENTER ], "");
  if (!oAttr.isEmpty()) moEnterActions = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_EXIT ], "");
  if (!oAttr.isEmpty()) moExitActions = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_TSTARTENTER], "");
  if (!oAttr.isEmpty()) moEnterStartTimers = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_TSTARTEXIT], "");
  if (!oAttr.isEmpty()) moExitStartTimers = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_TSTOPENTER], "");
  if (!oAttr.isEmpty()) moEnterStopTimers = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_TSTOPENTER], "");
  if (!oAttr.isEmpty()) moExitStopTimers = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_EVENTENTER], "");
  if (!oAttr.isEmpty()) moEnterEvent = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_EVENTEXIT], "");
  if (!oAttr.isEmpty()) moExitEvent = oAttr;

  // x
  oAttr = roInElement.attribute( gmaAttributeNames[ AN_X], "");
  if ( !oAttr.isEmpty())
  {
    setX( oAttr.toDouble());
  }
  // y
  oAttr = roInElement.attribute( gmaAttributeNames[ AN_Y], "");
  if ( !oAttr.isEmpty())
  {
    setY( oAttr.toDouble());
  }
#if 0
  // width
  oAttr = roInElement.attribute( gmaAttributeNames[ AN_WIDTH], "");
  if ( !oAttr.isEmpty()) ( oAttr.toDouble());
  // height
  oAttr = roInElement.attribute( gmaAttributeNames[ AN_HEIGHT], "");
  if ( !oAttr.isEmpty()) ( oAttr.toDouble());
#endif

  // add reference
  const QDomNode& roDomNode = roInElement.parentNode();
  if ( !roDomNode.isNull() )
  {
    if ( roDomNode.isElement() )
    {

      QString oTag = roDomNode.toElement().tagName();
      QString oName = roDomNode.toElement().attribute( "name", "");

      if ( oTag == FSMElementManager::gmaXMLSectionNames[ FSMElementManager::XE_FSM] )
      {
        setDomParentId( "");
      }
      else if ( oTag == FSMElementManager::gmaXMLSectionNames[ FSMElementManager::XE_STATE])
      {
        setDomParentId(
          roDomNode.toElement().attribute(gmaAttributeNames[ AN_NAME ], "" ) );
      }
    }
  }
}



// calculate id by Element class using related Dom Element
QString Node::calcId( const QDomElement& roInElement )
{
  QString oResult;

  if ( !roInElement.isNull() )
  {
    oResult = roInElement.attribute( gmaAttributeNames[ AN_NAME ], "" );
  }

  return oResult;
}


