/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#include <QtGui>
#include <QDomElement>
#include <QGraphicsRectItem>

#include "node.h"
#include "port.h"
#include "fsmelementmanager.h"

#define ENTRY_DISTANCE 10
#define ENTRY_WIDTH 5

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
  , mdSectionPadding( 2.0 )
  , moType( "" )
  , moEnterActions()
  , moExitActions()
  , moEnterStartTimers()
  , moExitStartTimers()
  , moEnterStopTimers()
  , moExitStopTimers()
  , moEnterEvents()
  , moExitEvents()
  , mpoSelectionHandleTopLeft( 0 )
  , mpoSelectionHandleTopRight( 0 )
  , mpoSelectionHandleBottomLeft( 0 )
  , mpoSelectionHandleBottomRight( 0 )
{
  if (bInIsPrototype ) return; // no further registration

  // create selection handles
  createSelectionHandles();

  moId = roInDefaultName;

  if ( poInDomElement )
  {
    applyAttributes( *poInDomElement );
  }
  moTextColor = Qt::darkGreen;
  moOutlineColor = Qt::darkBlue;
  moBackgroundColor = Qt::white;

  setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges );

  // create selection handles
  createSelectionHandles();

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
  QRectF rect = outlineRect().adjusted(-Margin, -Margin, +Margin, +Margin);
  if( "entry" == moType )
  {
    rect.setTopLeft(QPoint(rect.topLeft().x(),rect.topLeft().y()- ENTRY_DISTANCE - ENTRY_WIDTH));
  }
  return rect;
}

QPainterPath Node::shape() const
{
  QRectF rect = outlineRect();

  QPainterPath path;
  path.addRoundRect(rect, roundness(rect.width()),
                    roundness(rect.height()));
  QPoint oEllipseCenter = QPoint(rect.topLeft().x(),rect.topLeft().y()- ENTRY_DISTANCE);
  path.addEllipse(oEllipseCenter,ENTRY_WIDTH,ENTRY_WIDTH);
  return path;
}

// creates a port for connection/link to poToNode
Port* Node::createConnectionPort( const Node* poToNode )
{
  if ( !poToNode ) return 0;
 // if( poToNode->isEnterNode() && (1 < poToNode->portCnt())) return 0;

  bool bSelfTransition = false;

  // create new port
  Port* poPort = new Port( miPortCnt++, this );

  if( poToNode == this)
  {
    // self transition, consider special positions or mapping
    bSelfTransition = true;
  }

  // calculate position of the port
  QPointF oIntersectionPoint;
  QPolygonF oPolygon = shape().toFillPolygon();

  if( !bSelfTransition )
  {
    QLineF oLine(pos(), poToNode->pos());
    QPolygonF oPolygon = shape().toFillPolygon();
    QPointF p1 = oPolygon.first() + pos();
    QPointF p2;
    QLineF oSegment;
    for (int i = 1; i < oPolygon.count(); ++i)
    {
      p2 = oPolygon.at(i) + pos();
      oSegment = QLineF(p1, p2);
      QLineF::IntersectType oIntersectType;
        oIntersectType = oSegment.intersect(oLine, &oIntersectionPoint);
      if (oIntersectType == QLineF::BoundedIntersection)
        break;
      p1 = p2;
    }
  }
  else
  {
    oIntersectionPoint = oPolygon.first() + pos();
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

QString Node::getEnterActionsStr() const
{
  QString oText;
  QStringList list = moEnterActions.split(";",QString::SkipEmptyParts);
  foreach(QString str,list)
  {
    oText += "=>" + str + "\n";
  }
  return oText;
}

QString Node::getEnterEventsStr() const
{
  QString oText;
  QStringList list = moEnterEvents.split(";",QString::SkipEmptyParts);
  foreach(QString str,list)
  {
    oText += "^" + str +"\n";
  }
  return oText;
}

QString Node::getExitEventsStr() const
{
  QString oText;
  QStringList list = moExitEvents.split(";",QString::SkipEmptyParts);
  foreach(QString str,list)
  {
    oText += "^" + str +"\n";
  }
  return oText;
}


QString Node::getExitActionsStr() const
{
  QString oText;
  QStringList list = moExitActions.split(";",QString::SkipEmptyParts);
  foreach(QString str,list)
  {
    oText += "<=" + str + "\n";;
  }
  return oText;
}

QString Node::getEnterStartTimersStr() const
{
  QString oText;
  QStringList list = moEnterStartTimers.split(";",QString::SkipEmptyParts);
  foreach(QString str,list)
  {
    oText += "+" + str + "\n";;
  }
  return oText;
}

QString Node::getEnterStopTimersStr() const
{
  QString oText;
  QStringList list = moEnterStopTimers.split(";",QString::SkipEmptyParts);
  foreach(QString str,list)
  {
    oText += "-" + str + "\n";;
  }
  return oText;
}

QString Node::getExitStartTimersStr() const
{
  QString oText;
  QStringList list = moExitStartTimers.split(";",QString::SkipEmptyParts);
  foreach(QString str,list)
  {
    oText += "+" + str + "\n";;
  }
  return oText;
}

QString Node::getExitStopTimersStr() const
{
  QString oText;
  QStringList list = moExitStopTimers.split(";",QString::SkipEmptyParts);
  foreach(QString str,list)
  {
    oText += "-" + str + "\n";;
  }
  return oText;
}


void Node::paint(QPainter *painter,
                 const QStyleOptionGraphicsItem *option,
                 QWidget * /* widget */)
{
  QPen pen(moOutlineColor);
  QPen penSections(moOutlineColor);
  if (option->state & QStyle::State_Selected) {
    pen.setStyle(Qt::DotLine);
    pen.setWidth(2);
  }
  painter->setPen(pen);
  painter->setBrush(moBackgroundColor);

  QRectF oRect = outlineRect();
  painter->drawRoundRect(oRect, roundness(oRect.width()),
                         roundness(oRect.height()));

  if("entry" == moType)
  {
    painter->drawLine(  QPoint(oRect.topLeft().x()+oRect.width()/2, oRect.topLeft().y())
                       ,QPoint(oRect.topLeft().x()+oRect.width()/2, oRect.topLeft().y()-ENTRY_DISTANCE ) );
    painter->setBrush(Qt::black);
    painter->drawEllipse(QPoint(oRect.topLeft().x()+oRect.width()/2, oRect.topLeft().y()- ENTRY_DISTANCE ),ENTRY_WIDTH,ENTRY_WIDTH);
    painter->setBrush(moBackgroundColor);
  }

  painter->setPen(moTextColor);
  QString oText = moId;

  if(hasEnterFunc() || hasExitFunc())
  {
    QRectF oNameRect = getNameRect();
    QRectF oEnterRect = getEnterSectionRect();
    QRectF oExitRect = getExitSectionRect();
    qreal height1=getNameRect().height() + mdPadding;
    qreal height2=height1 + getEnterSectionRect().height();
    qreal height3=height2 + getExitSectionRect().height();
    painter->drawText(QRectF(oRect.left(),oRect.top(),oRect.width(),height1), Qt::AlignCenter, oText.trimmed() );
    painter->setPen(penSections);
    painter->drawLine(QPointF(oRect.left(),oRect.top() + height1),QPoint(oRect.right(),oRect.top() + height1));

    oText = getEnterActionsStr();
    oText += getEnterStartTimersStr();
    oText += getEnterStopTimersStr();
    oText += getEnterEventsStr();

    painter->setPen(moTextColor);
    painter->drawText(QRectF(oRect.left(),oRect.top() + height1,oRect.width(),getEnterSectionRect().height()), Qt::AlignCenter, oText.trimmed() );

    painter->setPen(penSections);
    painter->drawLine(QPointF(oRect.left(),oRect.top() + height2),QPoint(oRect.right(),oRect.top() + height2));
    painter->setPen(moTextColor);
    oText = getExitActionsStr();
    oText += getExitStartTimersStr();
    oText += getExitStopTimersStr();
    oText += getExitEventsStr();

    painter->drawText(QRectF(oRect.left(),oRect.top() + height2,oRect.width(),getExitSectionRect().height()), Qt::AlignCenter, oText.trimmed() );
   // painter->setPen(penSections);
   // painter->drawLine(QPointF(oRect.left(),oRect.top() + height3),QPoint(oRect.right(),oRect.top() + height3));
  }
  else
  {
    painter->drawText(oRect, Qt::AlignCenter, oText.trimmed() );
  }
}


bool Node::hasEnterFunc() const
{
  return (!moEnterActions.isEmpty() ||
          !moEnterStartTimers.isEmpty() ||
          !moEnterStopTimers.isEmpty() ||
          !moEnterEvents.isEmpty() );
}

bool Node::hasExitFunc() const
{
  return (!moExitActions.isEmpty() ||
          !moExitStartTimers.isEmpty() ||
          !moExitStopTimers.isEmpty() ||
          !moExitEvents.isEmpty() );
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

    // update selection handles
    updateHandlePositions();

    // update scene attributes
    QDomElement oDomDummy;
    updateAttributesScene( oDomDummy );
  } else if ( eInChange == ItemSelectedChange ) {
    // selection has changed
    setSelectionHandlesVisible( roInValue.toBool() );
  }

  // do work of base class
  return QGraphicsItem::itemChange(eInChange, roInValue);
}

QRectF Node::getNameRect() const
{
  QFontMetricsF metrics = qApp->font();
  QRectF oRect    = metrics.boundingRect(moId);
  if( hasEnterFunc() || hasExitFunc())
  {
    oRect.setHeight(oRect.height()+mdSectionPadding);
  }
  return oRect;

}

QRectF Node::getEnterSectionRect() const
{
  QRectF oRect;
  QRectF retRect;
  qreal dWidth = 0;

  if( hasEnterFunc())
  {
    QFontMetricsF metrics = qApp->font();
    QStringList list = getEnterActionsStr().split("\n",QString::SkipEmptyParts);
    list += getEnterStartTimersStr().split("\n",QString::SkipEmptyParts);
    list += getEnterStopTimersStr().split("\n",QString::SkipEmptyParts);
    list += getEnterEventsStr().split("\n",QString::SkipEmptyParts);

    foreach(QString str,list)
    {
      oRect = metrics.boundingRect( str );
      dWidth = qMax(oRect.width(),dWidth);
      retRect.setHeight(retRect.height() + oRect.height());
    }

    retRect.setWidth(dWidth);
    retRect.setHeight(retRect.height()+2*mdSectionPadding);
  }

  return retRect;
}

QRectF Node::getExitSectionRect() const
{
  QRectF oRect;

  QRectF retRect;
  qreal dWidth = 0;

  if( hasEnterFunc())
  {
    QFontMetricsF metrics = qApp->font();
    QStringList list = getExitActionsStr().split("\n",QString::SkipEmptyParts);
    list += getExitStartTimersStr().split("\n",QString::SkipEmptyParts);
    list += getExitStopTimersStr().split("\n",QString::SkipEmptyParts);
    list += getExitEventsStr().split("\n",QString::SkipEmptyParts);

    foreach(QString str,list)
    {
      oRect = metrics.boundingRect( str );
      dWidth = qMax(oRect.width(),dWidth);
      retRect.setHeight(retRect.height() + oRect.height());
    }
    retRect.setWidth(dWidth);
    retRect.setHeight(retRect.height()+2*mdSectionPadding);
  }

  return retRect;
}


QRectF Node::outlineRect() const
{
  enum TextType_T
  {
      TT_NAME
    , TT_ENTER
    , TT_EXIT
    , TT_LAST
  };

  QRectF oRectArr[ TT_LAST ];

  QFontMetricsF metrics = qApp->font();
  oRectArr[ TT_NAME ]     = getNameRect();
  oRectArr[ TT_ENTER ] = getEnterSectionRect();
  oRectArr[ TT_EXIT ] = getExitSectionRect();

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
  oRectText.setHeight( //3*mdPadding/2 +
          oRectArr[ TT_NAME ].height()
        + oRectArr[ TT_ENTER ].height()
        + oRectArr[ TT_EXIT ].height()
        );

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
  roInOutElement.setAttribute( gmaAttributeNames[ AN_EVENTENTER ], moEnterEvents );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_EVENTEXIT ], moExitEvents );

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
    scene()->update();
    updateHandlePositions();

  }

  return bResult;
}

// apply attributes
void Node::applyAttributes( const QDomElement& roInElement )
{
  QString oAttr = roInElement.attribute( gmaAttributeNames[ AN_NAME ], "" );
  if (!oAttr.isEmpty()) moId = oAttr;

  oAttr = roInElement.attribute( gmaAttributeNames[ AN_TYPE ], "" );
  moType = oAttr;

  oAttr = roInElement.attribute( gmaAttributeNames[ AN_ENTER ], "");
  moEnterActions = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_EXIT ], "");
  moExitActions = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_TSTARTENTER], "");
  moEnterStartTimers = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_TSTARTEXIT], "");
  moExitStartTimers = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_TSTOPENTER], "");
  moEnterStopTimers = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_TSTOPEXIT], "");
  moExitStopTimers = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_EVENTENTER], "");
  moEnterEvents = oAttr;
  oAttr  = roInElement.attribute( gmaAttributeNames[ AN_EVENTEXIT], "");
  moExitEvents = oAttr;

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


void Node::applySceneAttributes( const QDomElement& roInElement )
{
  // x
  QString oAttr = roInElement.attribute( gmaAttributeNames[ AN_X], "");
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
}


// create selection handles
void Node::createSelectionHandles()
{
  QRect oRect( -2.5,-2.5, 5.0, 5.0 );

  mpoSelectionHandleTopLeft     = new QGraphicsRectItem( this );
  mpoSelectionHandleTopLeft     -> setRect( oRect );
  mpoSelectionHandleTopLeft     ->
      setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges );

  mpoSelectionHandleTopRight    = new QGraphicsRectItem( this );
  mpoSelectionHandleTopRight    -> setRect( oRect );
  mpoSelectionHandleTopRight    ->
      setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges );

  mpoSelectionHandleBottomLeft  = new QGraphicsRectItem( this );
  mpoSelectionHandleBottomLeft  -> setRect( oRect );
  mpoSelectionHandleBottomLeft  ->
      setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges );

  mpoSelectionHandleBottomRight = new QGraphicsRectItem( this );
  mpoSelectionHandleBottomRight -> setRect( oRect );
  mpoSelectionHandleBottomRight ->
      setFlags(ItemIsMovable | ItemIsSelectable | ItemSendsGeometryChanges );

  setSelectionHandlesVisible( false );
}

// update handle positions
void Node::updateHandlePositions()
{
//  const qreal& rWidth = boundingRect().width();
//  const qreal& rHeight = boundingRect().height();

//  // move handles relative to center of Node
//  mpoSelectionHandleTopLeft     -> setPos( -rWidth / 2.0, -rHeight / 2.0 );
//  mpoSelectionHandleTopRight    -> setPos(  rWidth / 2.0, -rHeight / 2.0 );
//  mpoSelectionHandleBottomLeft  -> setPos( -rWidth / 2.0, rHeight / 2.0 );
//  mpoSelectionHandleBottomRight -> setPos(  rWidth / 2.0, rHeight / 2.0 );
  mpoSelectionHandleTopLeft -> setPos( boundingRect().topLeft() );
  mpoSelectionHandleTopRight -> setPos( boundingRect().topRight());
  mpoSelectionHandleBottomLeft -> setPos( boundingRect().bottomLeft());
  mpoSelectionHandleBottomRight -> setPos( boundingRect().bottomRight());

}

// show/hide handles
void Node::setSelectionHandlesVisible( bool bInShow )
{
  if ( mpoSelectionHandleTopLeft)
    mpoSelectionHandleTopLeft     -> setVisible( bInShow );
  if (mpoSelectionHandleTopRight)
    mpoSelectionHandleTopRight    -> setVisible( bInShow );
  if (mpoSelectionHandleBottomLeft)
    mpoSelectionHandleBottomLeft  -> setVisible( bInShow );
  if ( mpoSelectionHandleBottomRight )
  mpoSelectionHandleBottomRight -> setVisible( bInShow );
}

// calculate id by Element class using related Dom Element
// calculate id from a dom element
QString Node::calculateId( const QDomElement& roInElement) const
{
  QString oResult;

  if ( !roInElement.isNull() )
  {
    oResult = roInElement.attribute( gmaAttributeNames[ AN_NAME ], "" );
  }

  return oResult;
}


