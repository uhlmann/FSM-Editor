/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include <assert.h>

#include <QtGui>
#include <QLineF>
#include <QGraphicsSimpleTextItem>

#include "link.h"
#include "node.h"
#include "port.h"
#include "fsmelementmanager.h"

const qreal ARROWSIZE = 10;

int Link::gmiHelperCnt = 0;

const QString Link::gmaAttributeNames[ Link::AN_LAST ] =
{
  "id"          // AN_ID        id
  , "trigger"   // AN_TRIGGER   trigger for transition
  , "begin"     // AN_BEGIN     start state
  , "end"       // AN_END       destination state
  , "action"    // AN_ACTION    action to be performed on transition
  , "event"     // AN_EVENT     event sent when transition is performed
  , "condition" // AN_CONDITION condition for transition
  , "polygon"   // AN_POLYGON   list of point coordinates separated by spaces
};

// create and register prototype
static Link goLinkProto( 0,0, true, true );

Link::Link(Node *poInFromNode, Node *poInToNode, bool bInDirected, bool bInPrototype  )
  : FSMElementBase( 0, 0, gmaFSMElementTagName[ ET_TRANSITION], bInPrototype)
  , mpoTextItem( 0 )
  , moTrigger( QString("trigger_" ) + QString::number( Link::gmiHelperCnt++))
  , moStateFrom("")
  , moStateTo("")
  , moAction("")
  , moEvent("")
  , moCondition( "")
  , mbDrawArrow( bInDirected )
{
  if ( bInPrototype) return; // no further registration

  if ( !poInFromNode || !poInToNode)
  {/// destination or source node do not exist
    return;
  }

  mpoTextItem = new QGraphicsSimpleTextItem( this );

  // assign start node
  if ( poInFromNode ) moStateFrom = poInFromNode->getId();
  // assign end node
  if ( poInToNode)    moStateTo   = poInToNode->getId();

  mpoFromPort = poInFromNode->createConnectionPort( poInToNode );
  mpoToPort   = poInToNode->createConnectionPort( poInFromNode );

  mpoFromPort->addLink(this);
  mpoToPort->addLink(this);

  setFlags(QGraphicsItem::ItemIsSelectable);
  setZValue(-1);

  setColor(Qt::darkRed);
  updateGeometry();

  // update state names
  updateStates();

  // calculate id
  updateId();

  // update displayed text
  updateText();

  // add element to management
  FSMElementManager::getInstance().addElement( this );
}

Link::Link( const QDomElement * poInElement, bool bInDirected, bool bInPrototype )
  : FSMElementBase( 0,0, gmaFSMElementTagName[ ET_TRANSITION], bInPrototype )
  , mpoTextItem( 0 )
  , moTrigger( QString("trigger_" ) + QString::number( Link::gmiHelperCnt++))
  , moStateFrom("")
  , moStateTo("")
  , moAction("")
  , moEvent("")
  , moCondition( "")
  , mpoFromPort( 0 )
  , mpoToPort( 0 )
  , mbDrawArrow( bInDirected )
{
  if ( bInPrototype) return; // no further registration

  if (!poInElement) return;

  mpoTextItem = new QGraphicsSimpleTextItem( this );

  // search nodes and create ports
  applyAttributes( *poInElement );

  if ( !mpoFromPort || !mpoToPort ) return;
  mpoFromPort->addLink(this);
  mpoToPort->addLink(this);

  setFlags(QGraphicsItem::ItemIsSelectable);
  setZValue(-1);

  setColor(Qt::darkRed);
  updateGeometry();

  // calculate id
  updateId();

  // add element to management
  FSMElementManager::getInstance().addElement( this );
}

Link::~Link()
{
  if ( mpoFromPort )
  {
    mpoFromPort -> removeLink(this);
    if ( mpoFromPort->links().count() == 0 )
    {// no more links --> delete port
      delete mpoFromPort; mpoFromPort = 0;
    }
  }
  if ( mpoToPort   )
  {
    mpoToPort   -> removeLink(this);
    if ( mpoToPort->links().count() == 0)
    {// no more links --> delete port
      delete mpoToPort; mpoToPort = 0;
    }
  }
}

// release Port and set member pointer to 0 - no delete
// true if mpoFromPort or mpoToPort matches
bool Link::releasePort( Port* poInPort)
{
  if ( poInPort == mpoFromPort )
  {
    mpoFromPort = 0;
    return true;
  }
  if ( poInPort == mpoToPort)
  {
    mpoToPort = 0;
    return true;
  }

  return false;
}


Port *Link::fromPort() const
{
  return mpoFromPort;
}

Port *Link::toPort() const
{
  return mpoToPort;
}

void Link::setColor(const QColor &color)
{
  setPen(QPen(color, 1.0));
}

QColor Link::color() const
{
  return pen().color();
}

void Link::updateGeometry()
{
  if ( !mpoFromPort || !mpoToPort ) return;
  QPointF oStartPoint = mpoFromPort->mapToScene( mpoFromPort->boundingRect().center() );//
  QPointF oEndPoint   = mpoToPort->mapToScene( mpoToPort->boundingRect().center() );
  QPainterPath oPath(oStartPoint);
  if( moControlPoint.isNull())
  {
    oPath.lineTo((oEndPoint));
  }
  else
  {
    oPath.quadTo(moControlPoint,oEndPoint);
  }
  setPath(oPath);
  //setLine(QLineF( oStartPoint, oEndPoint ) );

  if ( mpoTextItem)
  {// move text item to center of arrow
//    QPointF oPos = mapToItem(this, boundingRect().center() );
    QPointF oPos = path().pointAtPercent(0.5) ;
    // move center of text object to center of line
//    oPos -= mapToItem( this, path().pointAtPercent(0.5) );
//    oPos -= mapToItem( this, mpoTextItem->boundingRect().center() );
    mpoTextItem->setPos( oPos );
  }

  // update scene attributes
  QDomElement oDomDummy;
  updateAttributesScene( oDomDummy );

  update();


}

/*
 update attribute moId
 precondition: call to updateStates()
*/
void Link::updateId()
{
  if ( mpoFromPort == 0 )
  {
    moId = "";
    return;
  }

  QString oId = moStateFrom;
  oId += "_";
  oId += moTrigger;
  oId += "_";
  oId += moCondition;

  // inform observers
  changedId( oId );
}

// update start and end state names
void Link::updateStates()
{
  // update name of start state
  if ( mpoFromPort )
  {
    FSMElementIfc* poFromState = mpoFromPort->getElementParent();
    if ( poFromState )
    {
      moStateFrom = poFromState->getId();
    }
  }

  // update name of destination state
  if ( mpoToPort )
  {
    FSMElementIfc* poToState = mpoToPort->getElementParent();
    if ( poToState )
    {
      moStateTo = poToState->getId();
    }
  }
}

// calculate id from a dom element
QString Link::calculateId( const QDomElement& roInElement) const
{
  // start state
  QString oStateFrom =
      roInElement.attribute( gmaAttributeNames[ AN_BEGIN ], moStateFrom );

  // trigger
  QString oTrigger =
      roInElement.attribute( gmaAttributeNames[ AN_TRIGGER ], moTrigger );

  // condition
  QString oCondition =
      roInElement.attribute( gmaAttributeNames[ AN_CONDITION], moCondition );

  QString oId = oStateFrom;
  oId += "_";
  oId += oTrigger;
  oId += "_";
  oId += oCondition;

  return oId;
}


QRectF Link::boundingRect() const
{
  if (!mpoFromPort || !mpoToPort)
    return QRectF();

  qreal penWidth = 1;
  qreal extra = (penWidth + ARROWSIZE) / 2.0;

  return QRectF(QGraphicsPathItem::boundingRect())
      .normalized()
      .adjusted(-extra, -extra, extra, extra);
}

void Link::paint
    (
        QPainter *                         poInPainter
        , const QStyleOptionGraphicsItem * poInOption
        , QWidget *                        poInWidget
        )
{
  if ( mbDrawArrow )
  {// draw arrow at end point
    QLineF oLineBack;
    oLineBack.setP1(path().pointAtPercent(0));
    oLineBack.setLength( ARROWSIZE );
    // rotate 15 degrees clockwise
    oLineBack.setAngle( path().angleAtPercent(0) + 15.0 );

    QPointF oArrowStart = oLineBack.p2();
    // rotate 2*15 degrees counter clockwise
    oLineBack.setAngle( oLineBack.angle() -30.0 );
    QPointF oArrowEnd = oLineBack.p2();

    QPolygonF oArrowHead;
    oArrowHead << oLineBack.p1() << oArrowStart << oArrowEnd;
    poInPainter->setPen(Qt::SolidLine);
    poInPainter->setBrush( Qt::SolidPattern );
    poInPainter->drawPolygon(oArrowHead);

  }
  // common work of base class
  QGraphicsPathItem::paint( poInPainter, poInOption, poInWidget);
}

// assign new text to text child item
void Link::updateText()
{
  if ( !mpoTextItem ) return;

  QString oText = moTrigger;
  if ( !moCondition.isEmpty())
  {
    oText +=" [";
    oText += moCondition;
    oText +="]";
  }

  if ( !moAction.isEmpty() )
  {
    oText += "\n";
    oText += moAction;
  }

  mpoTextItem->setText( oText );
  mpoTextItem->update();
}


const QString& Link::getId() const
{
  return moId;
}

// update dom attributes
void Link::updateAttributes( QDomElement& roInOutElement ) const
{
  if (roInOutElement.isNull() )
  {
    roInOutElement =
        FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_FSM, const_cast<Link*>( this ) );
  }

  if ( roInOutElement.isNull() ) return;

  roInOutElement.setAttribute( gmaAttributeNames[ AN_ID ],       moId );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_TRIGGER ],  moTrigger   );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_BEGIN ],    moStateFrom );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_END ],      moStateTo   );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_EVENT ],    moEvent     );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_ACTION],    moAction    );
  roInOutElement.setAttribute( gmaAttributeNames[ AN_CONDITION], moCondition    );
}

// assign new scene attributes apply derived methods
void Link::updateAttributesScene( QDomElement& roInOutElement) const
{
  if (roInOutElement.isNull() )
  {
    roInOutElement =
        FSMElementManager::getInstance().getDomElement( FSMElementManager::XS_SCENE, const_cast<Link*>( this ) );
  }

  if ( roInOutElement.isNull() ) return;

  roInOutElement.setAttribute( gmaAttributeNames[ AN_ID   ], getId() );
  if ( mpoFromPort && mpoToPort )
  {
    // store relative coordinate related to state parent
    QStringList oPolygon;
    oPolygon << QString::number(mpoFromPort->pos().x(), 'f');
    oPolygon << QString::number(mpoFromPort->pos().y(), 'f');
    oPolygon << QString::number(mpoToPort->pos().x(), 'f');
    oPolygon << QString::number(mpoToPort->pos().y(), 'f');
    oPolygon << QString::number(moControlPoint.x(), 'f');
    oPolygon << QString::number(moControlPoint.y(), 'f');

    roInOutElement.setAttribute( gmaAttributeNames[ AN_POLYGON], oPolygon.join( " "));
  }
}

// assign attribute - returning false if not assigned
bool Link::setAttribute( const QString& roInName, const QString& roInValue)
{
  bool bResult = false;
  if ( roInName == gmaAttributeNames[ AN_ID] )
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
void Link::applyAttributes( const QDomElement& roInElement )
{
  // id
  QString oAttr = roInElement.attribute( gmaAttributeNames[ AN_ID],      moId);
  if ( !oAttr.isNull() ) moId = oAttr;
  // trigger
  oAttr = roInElement.attribute( gmaAttributeNames[ AN_TRIGGER ], "" );
  if ( !oAttr.isNull() ) moTrigger = oAttr;
  // start state
  QString oStateFrom = roInElement.attribute( gmaAttributeNames[ AN_BEGIN ],   "" );
  if ( !oStateFrom.isNull() ) moStateFrom = oStateFrom;
  // destination state
  QString oStateTo = roInElement.attribute( gmaAttributeNames[ AN_END ],     "" );
  if ( !oStateTo.isNull() ) moStateTo = oStateTo;
  // action
  oAttr = roInElement.attribute( gmaAttributeNames[ AN_ACTION],   "" );
  if ( !oAttr.isNull() ) moAction = oAttr;
  // event
  oAttr = roInElement.attribute( gmaAttributeNames[ AN_EVENT ], "" );
  if ( !oAttr.isNull() ) moEvent = oAttr;
  // condition
  oAttr = roInElement.attribute( gmaAttributeNames[ AN_CONDITION], "");
  if ( !oAttr.isNull() ) moCondition = oAttr;

  if ( !oStateFrom.isEmpty() && !oStateTo.isEmpty() )
  {
    Node* poFromNode = dynamic_cast<Node*>( FSMElementManager::getInstance().getItem( moStateFrom ) );
    Node* poToNode   = dynamic_cast<Node*>( FSMElementManager::getInstance().getItem( moStateTo) );

    if (!poFromNode || !poToNode)
    {// exit states were not defined - todo message box
      return;
    }
    mpoFromPort = poFromNode->createConnectionPort( poToNode );
    mpoToPort   = poToNode->createConnectionPort( poFromNode );

    // assign text to text item child
    updateText();
  }

  if ( !mpoFromPort || !mpoToPort ) return;

  QString oPolygon = roInElement.attribute( gmaAttributeNames[ AN_POLYGON], "");
  if ( !oPolygon.isEmpty() )
  {
    QStringList oPointList = oPolygon.split(" ");
    if ( mpoFromPort && oPointList.count() >= 2 )
    {
      bool bValid = false;
      double dX = (oPointList.takeFirst()).toDouble( &bValid );
      if ( bValid )
      {
        double dY = (oPointList.takeFirst()).toDouble( &bValid );
        if ( bValid )
        {

          mpoFromPort->setPos( dX, dY );
          mpoFromPort->update();
        }
        else
        {
          assert( 0 );
        }
      }
      else
      {
        assert( 0 );
      }
    }

    if ( mpoToPort && oPointList.count() >= 2 )
    {
      bool bValid = false;
      double dX = (oPointList.takeFirst()).toDouble( &bValid );
      if ( bValid )
      {
        double dY = (oPointList.takeFirst()).toDouble( &bValid );
        if ( bValid )
        {
          mpoToPort->setPos( dX, dY );
          mpoToPort->update();
        }
        else
        {
          assert( 0 );
        }
      }
      else
      {
        assert( 0 );
      }
    }

    if( oPointList.count() >= 2 )
    {
      bool bValid = false;
      double dX = (oPointList.takeFirst()).toDouble( &bValid );
      if ( bValid )
      {
        double dY = (oPointList.takeFirst()).toDouble( &bValid );
        if ( bValid )
        {
          moControlPoint.setX(dX);
          moControlPoint.setY(dY);
        }
        else
        {
          assert( 0 );
        }
      }
      else
      {
        assert( 0 );
      }
    }
  }
  updateGeometry();
  // add reference to parent dom node
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
            roDomNode.toElement().attribute(Node::gmaAttributeNames[ Node::AN_NAME ], "" ) );
      }
    }
  }
}

// slot to connect on signal sigChangedId( const FSMElementIfc&)
void Link::slotUpdateId( const FSMElementIfc& )
{
  // update start and end state
  updateStates();
  // update id and notify observers by emitting signal
  updateId();
}


void Link::mouseDoubleClickEvent ( QGraphicsSceneMouseEvent * event )
{
  if(event && !event->pos().isNull())
  {
    moControlPoint=QPointF(0,0);
    updateGeometry();
    //setPath(oPath);
  }
}

void Link::mouseMoveEvent ( QGraphicsSceneMouseEvent * event )
{
  if(event && !event->pos().isNull())
  {
//    QPointF oStart = path().pointAtPercent(1);
//    QPointF oEnd = path().pointAtPercent(0);
//    QPainterPath oPath(oStart);
//    oPath.quadTo(event->pos(),oEnd);
//    setPath(oPath);
    moControlPoint = event->pos();
    updateGeometry();
  }
}


