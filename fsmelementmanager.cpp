/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#include <assert.h>
#include <QApplication>
#include <QDomNodeList>
#include <QGraphicsItem>
#include <QMessageBox>
#include <QTextStream>
#include <QTreeWidgetItem>
#include "fsmelementmanager.h"
#include "definitions/fsmdefinitionifc.h"
#include "definitions/fsmdefinitionbase.h"
#include "elements/fsmelementifc.h"
#include "elements/fsmelementbase.h"
#include "mainwin.h"

const QString FSMElementManager::gmaXMLSectionNames[ FSMElementManager::XE_LAST] =
{
  "definitions"
      , "fsm"
      , "scene"
      , "action"     // XE_ACTION  action
      , "trigger"    // XE_TRIGGER trigger
      , "timer"      // XE_TIMER   timer
      , "state"      // XE_STATE   state
      , "transition" // XE_TRANSITION transition
    };

const QString FSMElementManager::moDocumentName = "fsm_ui";
const QString FSMElementManager::moRootName     = "root";
// counter for definitions
unsigned int FSMElementManager::guiDefCnt       = 0;


FSMElementManager::FSMElementManager(QObject *parent) :
    QObject(parent)
    , mpoMainWindow( 0 )
    , moLevel("")
    , moDomDocumentWrite( moDocumentName )
    , mpoRootTreeItem( 0 )
    , mpoRootTreeItemDefinitions( 0 )
    , moItemToTreeItem()
    , moDefinitionToTreeItem()
{
  moRootElement = moDomDocumentWrite.createElement( moRootName );
  moDomDocumentWrite.appendChild( moRootElement );
  // section defintion
  moRootDefinitionElement =
      moDomDocumentWrite.createElement( gmaXMLSectionNames[ XE_DEFINITIONS]);
  moRootElement.appendChild( moRootDefinitionElement);
  // section fsm
  moRootFSMElement =
      moDomDocumentWrite.createElement( gmaXMLSectionNames[ XE_FSM]);
  moRootElement.appendChild( moRootFSMElement);
  // section scene
  moRootSceneElement =
      moDomDocumentWrite.createElement( gmaXMLSectionNames[ XE_SCENE]);
  moRootElement.appendChild( moRootSceneElement);
}

FSMElementManager::~FSMElementManager()
{
}

FSMElementManager& FSMElementManager::getInstance()
{
  static FSMElementManager goInstance;
  return goInstance;
}

// register main window to position message boxes etc
bool FSMElementManager::addMainWin( MainWin* poInMainWin )
{
  if ( !poInMainWin || mpoMainWindow ) return false;

  mpoMainWindow = poInMainWin;

  /// actions to be performed when main window is destroyed
  connect( mpoMainWindow, SIGNAL( destroyed() ), this, SLOT( slotMainWinDestroyed() ) );

  return true;
}

// add definition to be managed - returns true on success
bool FSMElementManager::addDefinition( FSMDefinitionIfc* poInDefinition)
{
  if ( !poInDefinition ) return false;
  if ( moDefinitions.contains( poInDefinition->getName() ) ) return false;

  // add definition to map
  moDefinitions[ poInDefinition->getName()] = poInDefinition;

  // add definition info
  addDomDefinitionInfo( poInDefinition );
  // create tree item and add to navigation tree
  addTreeItem( poInDefinition );

  return true;
}

// add element to be managed - returns true on success
bool FSMElementManager::addElement( FSMElementIfc* poInElement )
{
  if ( !poInElement ) return false;


  if ( moElements.contains( poInElement->getId() ) )
  {
    return false;
  }

  moElements[ poInElement->getId() ] = poInElement;

  // add fsm info
  addDomFSMInfo( poInElement);
  // add scene related info
  addDomSceneInfo( poInElement );
  // create tree item and add to navigation tree
  addTreeItem( poInElement );

  return true;
}

// add info to fsm section of dom
void FSMElementManager::addDomFSMInfo( FSMElementIfc* poInElement )
{
  if (!poInElement) return; // sorry - no element

  QDomElement oDomElement = poInElement->createDomElement( moDomDocumentWrite );
  if ( !oDomElement.isNull() )
  {// add element to xml document
    moItemToDomElement[XS_FSM][ poInElement ] = oDomElement;
    FSMElementIfc* poParent = poInElement->getElementParent();
    if (!poParent )
    {// parent is null node - so add to root element
      const QString& roDomParentId = poInElement->getDomParentId();
      if ( moElements.contains( roDomParentId ) )
      {
        poParent = moElements[ roDomParentId ];
      }
      else if ( moElements.contains( getLevel()) )
      {// add to current level
        poParent = moElements[ getLevel() ];
      }
    }

    if ( !poParent )
    {// append to fsm section
      moRootFSMElement.appendChild( oDomElement );
    }
    else
    {// append as child of element
      QDomElement oDomParent = getDomElement( XS_FSM, poParent );
      if ( !oDomParent.isNull() )
      {
        poInElement->setDomParentId( poParent->getId() );
        oDomParent.appendChild( oDomElement);
      }
    } // end if !oDomParent.isNull()
  }
}

// add info to definition sectio of dom
void FSMElementManager::addDomDefinitionInfo( FSMDefinitionIfc* poInDefinition)
{
  if (!poInDefinition) return; // sorry - no definition

  QDomElement oDomElement = poInDefinition->createDomElement( moDomDocumentWrite );
  if ( !oDomElement.isNull() )
  {// add element to xml document
    // assign new attributes apply derived method
    poInDefinition->updateAttributes( oDomElement );
    // insert defintion in hash
    moItemToDomElement[XS_DEFINITIONS][ poInDefinition ] = oDomElement;
    // append to definition section
    moRootDefinitionElement.appendChild( oDomElement );
  }
}


// add info to scene section of dom
void FSMElementManager::addDomSceneInfo( FSMElementIfc* poInElement )
{
  QDomElement oDomElement = poInElement->createDomSceneElement(moDomDocumentWrite );
  if ( !oDomElement.isNull() )
  {// add element to xml document
    moItemToDomElement[ XS_SCENE][ poInElement ] = oDomElement;
    moRootSceneElement.appendChild( oDomElement );
  }
}

// create tree item and add it to the navigation tree
void FSMElementManager::addTreeItem( FSMElementIfc* poInElement )
{
  if (!poInElement) return;
  // only states should be represented as tree items
  if ( poInElement->getType() != FSMElementIfc::ET_STATE ) return;
  // create new tree item

  // check if element is top level element
  const QString& roParentId = poInElement->getDomParentId();
  if ( roParentId.isEmpty() )
  {// root element
    if ( !mpoRootTreeItem )
    {
      mpoRootTreeItem =
        new QTreeWidgetItem( mpoMainWindow->getLevelView());
      mpoRootTreeItem->setText( 0, moRootName );
    }
    // create new item
    QTreeWidgetItem* poTreeItem =
        new QTreeWidgetItem( mpoRootTreeItem );
    //  assign text
    poTreeItem->setText( 0, poInElement->getId() );
    // add to hash
    moItemToTreeItem[ poInElement ] = poTreeItem;
  }
  else
  {
    if ( moElements.contains( roParentId ))
    {
      FSMElementIfc* poSceneItem = moElements[ roParentId ];
      if ( moItemToTreeItem.contains( poSceneItem ))
      {
        QTreeWidgetItem* poTreeItemParent = moItemToTreeItem[ poSceneItem ];
        QTreeWidgetItem* poTreeItem =
            new QTreeWidgetItem( poTreeItemParent );
        //  assign text
        poTreeItem->setText( 0, poInElement->getId() );
        // add to hash
        moItemToTreeItem[ poInElement ] = poTreeItem;
      }
      else
      {// should not occur - parent id is assigned so element should exist
        assert( 0 );
      }
    }
    else
    {// assuming parent element is already registered - so this should not occur
      assert( 0 );
    }
  }
}

// create tree item and add it to the defintion tree
void FSMElementManager::addTreeItem( FSMDefinitionIfc* poInDefinition )
{
  if (!poInDefinition) return;

  if ( !mpoRootTreeItemDefinitions )
  {// secion missing
    mpoRootTreeItemDefinitions =
        new QTreeWidgetItem( mpoMainWindow->getDefinitionView());
    mpoRootTreeItemDefinitions->setText( 0, moRootName );
  }
  // create new item
  QTreeWidgetItem* poTreeItem =
      new QTreeWidgetItem( mpoRootTreeItemDefinitions );
  //  assign text
  poTreeItem->setText( 0, poInDefinition->getTypeName() );
  poTreeItem->setText( 1, poInDefinition->getName()     );
  // add to hash
  moDefinitionToTreeItem[ poInDefinition ] = poTreeItem;
}

// return definition by id, or 0 if non existant
FSMDefinitionIfc* FSMElementManager::getDefinition( const QString& roInName ) const
{
  if ( moDefinitions.contains( roInName ) )
  {
    return moDefinitions[ roInName ];
  }

  return 0; /// no entry found with that name
}


// return definition by related tree item, or 0 if not related
DomViewIfc* FSMElementManager::getDefinition( const QTreeWidgetItem* poInItem) const
{
  if ( moDefinitionToTreeItem.isEmpty() ) return 0;

  ElementToTreeItem_T::ConstIterator oItr = moDefinitionToTreeItem.begin();
  while ( oItr != moDefinitionToTreeItem.end() )
  {
    if ( oItr.value() == poInItem )
    {
      return oItr.key();
    }
    ++oItr; // next element
  }

  return 0; // no match found

}



// return item by id, or 0 if not existant
FSMElementIfc* FSMElementManager::getItem( const QString& roInId) const
{
  if (!moElements.contains( roInId ) ) return 0;

  return moElements[ roInId ];
}

// returns registered dom node or 0, if not registered
QDomElement FSMElementManager::getDomElement( XMLSections_T eInSection, DomViewIfc* poInElement )
{
  if ( !moItemToDomElement[ eInSection].contains(poInElement) )
  {
    static const QDomElement oErrorElement;
    return oErrorElement;
  }

  return moItemToDomElement[eInSection][ poInElement ];
}

// actions to apply when id changes
void FSMElementManager::changedId
(
    FSMElementIfc& roInOutElement
    , const QString& roInNewId
    , const QString& roInOldId
)
{
  if (!moElements.contains( roInOldId)) return;

  moElements.remove( roInOldId );

  // add element with new id
  moElements[ roInNewId ] = &roInOutElement;

  // replace parent id
  foreach ( FSMElementIfc* poElement, moElements )
  {
    if ( poElement && ( poElement->getDomParentId() == roInOldId ) )
    {// parents id matches and has to be replaces
      poElement->setDomParentId( roInNewId );
    }
  }

  // update level
  if ( moLevel == roInOldId)
  {
    setLevel( roInNewId );
  }

  // update text in item in navigation tree (levels)
  if ( moItemToTreeItem.contains( &roInOutElement ))
  {
    QTreeWidgetItem* poTreeItem =
        moItemToTreeItem[ &roInOutElement];
    if ( poTreeItem )
    {
      poTreeItem->setText( 0, roInNewId );
    }
  }
}

// actions to apply when id of an definitioin changes
void FSMElementManager::changedId
(
    FSMDefinitionIfc& roInOutElement
    , const QString& roInNewId
    , const QString& roInOldId
)
{
  if (!moDefinitions.contains( roInOldId)) return;

  moDefinitions.remove( roInOldId );

  // add element with new id
  moDefinitions[ roInNewId ] = &roInOutElement;

  // update text in item in navigation tree (levels)
  if ( moDefinitionToTreeItem.contains( &roInOutElement ))
  {
    QTreeWidgetItem* poTreeItem =
        moDefinitionToTreeItem[ &roInOutElement];
    if ( poTreeItem )
    {
      poTreeItem->setText( 1, roInNewId );
    }
  }
}

// read fsm defintion from an xml file
bool FSMElementManager::read( QIODevice* poInDevice )
{
  QString oErrorString("");
  int iErrorLine   = -1;
  int iErrorColumn = -1;

  if (!moDomDocumentRead.setContent(poInDevice, true, &oErrorString, &iErrorLine,
                                    &iErrorColumn))
  {
    QMessageBox::information( mpoMainWindow, tr("FSM Definition"),
                              tr("Parse error at line %1, column %2:\n%3")
                              .arg(iErrorLine)
                              .arg(iErrorColumn)
                              .arg(oErrorString));
    return false;
  }

  QDomElement oRootElement = moDomDocumentRead.documentElement();
  if (oRootElement.tagName() != moRootName) {
    QMessageBox::information( mpoMainWindow, tr("FSM Definition"),
                              tr("The file is not an FSM definition file."));
    return false;
  } else if (oRootElement.hasAttribute("version")
    && oRootElement.attribute("version") != "1.0") {
    QMessageBox::information( mpoMainWindow, tr("FSM Definition"),
                              tr("The file is not an FSM definition version 1.0 "
                                 "file."));
    return false;
  }

  // parse section definitions, fsm, scene
  QDomElement oDefinitionElement =
      oRootElement.firstChildElement( gmaXMLSectionNames[ XE_DEFINITIONS ]);
  while (!oDefinitionElement.isNull())
  {
    parseDefinitionsElement(oDefinitionElement);
    oDefinitionElement = oDefinitionElement.nextSiblingElement(gmaXMLSectionNames[ XE_DEFINITIONS ]);
  }

  // parse section fsm
  QDomElement oFsmElement =
      oRootElement.firstChildElement( gmaXMLSectionNames[ XE_FSM ]);
  while (!oFsmElement.isNull())
  {
    parseFSMElement(oFsmElement);
    oFsmElement = oFsmElement.nextSiblingElement(gmaXMLSectionNames[ XE_FSM ]);
  }

  // parse section scene
  QDomElement oSceneElement =
      oRootElement.firstChildElement( gmaXMLSectionNames[ XE_SCENE ]);
  while (!oSceneElement.isNull())
  {
    parseSceneElement(oSceneElement);
    oSceneElement = oSceneElement.nextSiblingElement(gmaXMLSectionNames[ XE_SCENE ]);
  }

  // show root level
  setLevel( "", true );

  return true;
}

// write fsm definition as XML file
bool FSMElementManager::write( QIODevice* poInDevice )
{
  if (!poInDevice ) return false;

  const int iIndentSize = 4;

  QTextStream oOutStream(poInDevice);
  oOutStream.setCodec( "UTF-8" );
  moDomDocumentWrite.save(oOutStream, iIndentSize, QDomElement::EncodingFromTextStream);
  return true;
}

// parse section fsm and create default scene elements if the were not created
bool FSMElementManager::parseSceneElement( const QDomElement& roInElement)
{
  if (roInElement.isNull() ) return false;

  // scene elements to be parsed
  QList<XMLElements_T> oElementList;
  oElementList.append( XE_STATE );
  oElementList.append( XE_TRANSITION);
  // id of scene element
  QString oId = "";
  foreach ( XMLElements_T eElementType, oElementList )
  {
    // parse states
    QDomElement oSceneElement =
        roInElement.firstChildElement( gmaXMLSectionNames[ eElementType ]);
    while (!oSceneElement.isNull())
    {
      oId = oSceneElement.attribute( "id", "");
      if ( !oId.isEmpty())
      {
        FSMElementIfc* poItem = getItem( oId );
        if ( poItem )
        {// assgingn values contained in attributes
          poItem->applyAttributes( oSceneElement );
        }
      } // attribut id is assigned
      oSceneElement = oSceneElement.nextSiblingElement(gmaXMLSectionNames[ eElementType ]);
    } // while
  } // foreach


  return true;
}

// parse section definitions and create default scene elements if the were not created
bool FSMElementManager::parseDefinitionsElement( const QDomElement& roInElement)
{
  if (roInElement.isNull() ) return false;

  foreach (QString oProtoTypeTag, FSMDefinitionBase::getDefinitionNames() )
  {
    QDomElement oDefinitionElement =
        roInElement.firstChildElement( oProtoTypeTag );
    while ( !oDefinitionElement.isNull() )
    {
      FSMDefinitionIfc* poDefinition =
          FSMDefinitionBase::createByName( oProtoTypeTag, QString( "dummy_") + QString::number( guiDefCnt++));
      if ( poDefinition )
      {// map attributes to internal attributes of object
        poDefinition->applyAttributes( oDefinitionElement );
      }
      oDefinitionElement = oDefinitionElement.nextSiblingElement( oProtoTypeTag );
    }
  }

  return true;
}

// parse section fsm and create default scene elements if the were not created
bool FSMElementManager::parseFSMElement( const QDomElement& roInElement )
{
  if (roInElement.isNull() ) return false;

  // parse states
  QDomElement oStateElement =
      roInElement.firstChildElement( gmaXMLSectionNames[ XE_STATE ]);
  while (!oStateElement.isNull())
  {
    parseStateElement(oStateElement);
    oStateElement = oStateElement.nextSiblingElement(gmaXMLSectionNames[ XE_STATE ]);
  }

  // parse transitions
  QDomElement oTransitionsElement =
      roInElement.firstChildElement( gmaXMLSectionNames[ XE_TRANSITION ]);
  while (!oTransitionsElement.isNull())
  {
    parseTransitionElement(oTransitionsElement);
    oTransitionsElement = oTransitionsElement.nextSiblingElement(gmaXMLSectionNames[ XE_TRANSITION ]);
  }

  return true;
}

// parse state and create related graphic element
bool FSMElementManager::parseStateElement( const QDomElement& roInElement )
{
  // create scene object
  if ( mpoMainWindow )
  {
    mpoMainWindow->slotAddNode( &roInElement );
  }

  // parse substates
  QDomElement oStateElement =
      roInElement.firstChildElement( gmaXMLSectionNames[ XE_STATE ]);
  while (!oStateElement.isNull())
  {
    parseStateElement(oStateElement);
    oStateElement = oStateElement.nextSiblingElement(gmaXMLSectionNames[ XE_STATE ]);
  }

  // parse transitions
  QDomElement oTransitionsElement =
      roInElement.firstChildElement( gmaXMLSectionNames[ XE_TRANSITION ]);
  while (!oTransitionsElement.isNull())
  {
    parseTransitionElement(oTransitionsElement);
    oTransitionsElement = oTransitionsElement.nextSiblingElement(gmaXMLSectionNames[ XE_TRANSITION ]);
  }

  return true;
}

bool FSMElementManager::parseTransitionElement( const QDomElement& roInElement )
{
  // create scene object
  if ( mpoMainWindow )
  {
    mpoMainWindow->slotAddLink( &roInElement );
  }

  return true;
}

// enter new level - show / hide scene elements
void FSMElementManager::setLevel( const QString& roInLevel, bool bInForce )
{
  if ( ( moLevel == roInLevel ) && !bInForce ) return; // no change - no force

  // assing level as current level
  moLevel = roInLevel;

  QGraphicsItem* poGraphicsItem = 0;
  foreach ( FSMElementIfc* poSceneItem, moElements )
  {
    if ( poSceneItem )
    {
      poGraphicsItem = poSceneItem->getGraphicsItem();
      if ( poGraphicsItem )
      {
        QString oParentId = poSceneItem->getDomParentId();
        QString oName     = poSceneItem->getId();
        if ( roInLevel == oParentId )
        {
          poGraphicsItem->show();
        }
        else
        {
          if ( poSceneItem->getType() != FSMElementIfc::ET_PORT )
          {// visibility managed by parent (Node)
            poGraphicsItem->hide();
          }
        } // show / hide related to level
      } // related graphics item exits
    } // scene item is not null
  }
}

void FSMElementManager::slotElementDestroyed( QObject* poInObject )
{
  FSMElementBase* poItem = dynamic_cast<FSMElementBase*>( poInObject );
  // elements with no id is e.g. the link prototype
  if ( poItem && !poItem->getId().isEmpty())
  {
    if ( !moElements.contains( poItem->getId())) return; // element already removed

    // remove from hash
    moElements.remove( poItem->getId() );

    unsigned int uiIdx = 0;
    for ( uiIdx = 0; uiIdx < XS_LAST; ++uiIdx)
    {
      if (!moItemToDomElement[uiIdx].contains( poItem) ) continue; // skip rest of loop

      // remove item from map to dom element
      QDomElement roDomElement = moItemToDomElement[uiIdx].take( poItem );

      if ( !roDomElement.isNull() )
      {
        // get List with children
        QDomNodeList oChildList = roDomElement.childNodes();
        // remove element from parents
        QDomNode roParentNode = roDomElement.parentNode();
        if (!roParentNode.isNull())
        {// remove element from parent
          roParentNode.removeChild( roDomElement );
        }

        unsigned int uiIdx = 0;
        for ( uiIdx = 0; uiIdx < oChildList.length(); ++uiIdx )
        {
          if ( !oChildList.item( static_cast<int>( uiIdx ) ).isElement() ) continue;

          QString oId = FSMElementBase::getId( oChildList.item( static_cast<int>( uiIdx ) ).toElement() );
          // get related graphical item
          FSMElementBase* poItem = dynamic_cast<FSMElementBase*>( moElements[ oId ] );
          if ( poItem) {
            // destroy related graphic item
            delete poItem; poItem = NULL;
          } // if poItem
        } //
      }
    }
    if ( moItemToTreeItem.contains( poItem ))
    {
      QTreeWidgetItem* poTreeItem = moItemToTreeItem.take( poItem );
      delete poTreeItem; poTreeItem = 0;
    }
  }
  else
  {// check if object is a definition
    FSMDefinitionBase* poDefinition =
        dynamic_cast<FSMDefinitionBase*>( poInObject);
    if ( poDefinition)
    {
      moDefinitions.remove( poDefinition->getName() );

    }
  }
}

void FSMElementManager::slotMainWinDestroyed()
{
  mpoMainWindow = 0;
}
