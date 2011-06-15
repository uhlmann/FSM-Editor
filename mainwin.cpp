/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#include <assert.h>

#include <QtGui>
#include <QDockWidget>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include "mainwin.h"
#include "domviewifc.h"
#include "fsmelementmanager.h"
#include "definitions/fsmdefinitionifc.h"
#include "elements/link.h"
#include "elements/node.h"
#include "elements/port.h"
#include "dlg/dlgattributes.h"
#include "dlg/dlgproperties.h"
#include "dlg/dlgdefinitonnew.h"

MainWin::MainWin()
  : mpoFileMenu( 0 )
  , mpoEditMenu( 0 )
  , mpoViewMenu( 0 )
  , mpoEditToolBar( 0 )
  , mpoActionOpen( 0 )
  , mpoActionSaveAs( 0 )
  , mpoActionClose( 0 )
  , mpoActionExit( 0 )
  , mpoActionNode( 0 )
  , mpoActionLink( 0 )
  , mpoActionDefinition( 0 )
  , mpoActionDelete( 0 )
  , mpoActionCut( 0 )
  , mpoActionCopy( 0 )
  , mpoActionPaste( 0 )
  , mpoActionBringToFront( 0 )
  , mpoActionBringToBack( 0 )
  , mpoActionProperties( 0 )
  , mpoActionDefinitionsDelete( 0 )
  , mpoScene( new QGraphicsScene(0, 0, 5000, 5000) )
  , mpoView( new QGraphicsView )
  , mpoDefinitionNav( 0 )
  , mpoLevelNav( 0 )
  , mpoStateNav( 0 )
  , mpoDlgAttributes( 0 )
  , mpoDlgDefinitionNew( 0 )
{
  mpoView->setScene(mpoScene);
  mpoView->setDragMode(QGraphicsView::RubberBandDrag);
  mpoView->setRenderHints(QPainter::Antialiasing
                          | QPainter::TextAntialiasing);
  mpoView->setContextMenuPolicy(Qt::ActionsContextMenu);
  mpoView->centerOn( QPointF( 0,0 ) );
  setCentralWidget(mpoView);

  miMinZ = 0;
  miMaxZ = 0;
  miNumber = 0;

  createActions();
  createMenus();
  createToolBars();
  createNavigation();

  connect(mpoScene, SIGNAL(selectionChanged()),
          this, SLOT(slotSelectionChanged()));

  // assert level navigation is created successfully
  assert( mpoLevelNav );
  // handle new level selection
  connect(
      mpoLevelNav, SIGNAL( currentItemChanged( QTreeWidgetItem*, QTreeWidgetItem* ) )
      , this, SLOT( slotChangedLevel( QTreeWidgetItem*, QTreeWidgetItem* ) ) );

  setWindowTitle(tr("FSM Editor"));
  slotUpdateActions();

  FSMElementManager::getInstance().addMainWin( this );
}


void MainWin::slotOpen()
{
  QString oFileName =
      QFileDialog::getOpenFileName(this, tr("Open FSM Definition File"),
                                   QDir::currentPath(),
                                   tr("XML Files (*.xml)"));
  if (oFileName.isEmpty())
    return;

  QFile oFile(oFileName);
  if (!oFile.open(QFile::ReadOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("FSM Definition"),
                         tr("Cannot read file %1:\n%2.")
                         .arg(oFileName)
                         .arg(oFile.errorString()));
    return;
  }

  if(mpoScene) mpoScene->clear();
  if (FSMElementManager::getInstance().read(&oFile))
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWin::slotClose()
{
  FSMElementManager::getInstance().close();
  if( mpoScene )
  {
    mpoScene->clear();
  }
}


void MainWin::slotSaveAs()
{
  QString oFileName =
      QFileDialog::getSaveFileName(this, tr("Save FSM Definition"),
                                   QDir::currentPath(),
                                   tr("XML Files (*.xml)"));
  if (oFileName.isEmpty())
    return;

  QFile oFile(oFileName);
  if (!oFile.open(QFile::WriteOnly | QFile::Text)) {
    QMessageBox::warning(this, tr("FSM Definition"),
                         tr("Cannot write file %1:\n%2.")
                         .arg(oFileName)
                         .arg(oFile.errorString()));
    return;
  }


  if (FSMElementManager::getInstance().write(&oFile))
    statusBar()->showMessage(tr("File saved"), 2000);
}

void MainWin::slotAddNode( const QDomElement * poInElement )
{

  Node *poNode = new Node( poInElement, tr("Node %1").arg(miNumber + 1) );

  setupNode(poNode);
}

void MainWin::slotAddLink( const QDomElement * poInElement )
{
  Link *poLink = 0;
  if ( poInElement )
  {
    poLink = new Link( poInElement );
  }
  else
  {
    NodePair_T nodes = selectedNodePair();
    if (nodes == NodePair_T())
      return;

    poLink = new Link(nodes.first, nodes.second);
  }
  mpoScene->addItem(poLink);
}

// add a new definition
void MainWin::slotAddDefinition()
{
  if ( !mpoDlgDefinitionNew)
  {
    mpoDlgDefinitionNew = new DlgDefinitionNew( this);
  }

  mpoDlgDefinitionNew->show();
}

void MainWin::slotDelete()
{
  QList<QGraphicsItem *> oItems = mpoScene->selectedItems();
  QMutableListIterator<QGraphicsItem *> i(oItems);
  while (i.hasNext()) {
    Link *link = dynamic_cast<Link *>(i.next());
    if (link) {
      delete link;
      i.remove();
    }
  }

  qDeleteAll(oItems);
}

// delete currently selected definition
void MainWin::slotDeleteDefinition()
{
  if ( !mpoDefinitionNav ) return;

  if ( mpoDefinitionNav->selectedItems().isEmpty() ) return; // no element selected

  // \todo ask user to confirm delete action

  foreach ( QTreeWidgetItem* poItem, mpoDefinitionNav->selectedItems())
  {// delete all selected items
    if ( poItem)
    {
      //\todo identify and remove related dom elements
      // free memory
      delete poItem; poItem = 0;
    }
  }
}

void MainWin::slotChangedLevel( QTreeWidgetItem* poCurrent, QTreeWidgetItem* /*poPrev*/ )
{
  if ( !poCurrent ) return;

  const QString& roText = poCurrent->text( 0 );
  if ( roText == FSMElementManager::moRootName )
  {
    FSMElementManager::getInstance().setLevel( QString("") );
  }
  else
  {
    FSMElementManager::getInstance().setLevel( roText );
  }
}


void MainWin::slotCut()
{
  Node *node = selectedNode();
  if (!node)
    return;

  slotCopy();
  delete node;
}

void MainWin::slotCopy()
{
  Node *node = selectedNode();
  if (!node)
    return;

  QString str = QString("Node %1 %2 %3 %4")
                .arg(node->textColor().name())
                .arg(node->outlineColor().name())
                .arg(node->backgroundColor().name())
                .arg(node->name());
  QApplication::clipboard()->setText(str);
}

void MainWin::slotPaste()
{
  QString str = QApplication::clipboard()->text();
  QStringList parts = str.split(" ");

  if (parts.count() >= 5 && parts.first() == "Node") {
    Node *node = new Node( 0, QStringList(parts.mid(4)).join(" ") );
    node->setTextColor(QColor(parts[1]));
    node->setOutlineColor(QColor(parts[2]));
    node->setBackgroundColor(QColor(parts[3]));
    setupNode(node);
  }
}

void MainWin::slotBringToFront()
{
  ++miMaxZ;
  setZValue(miMaxZ);
}

void MainWin::slotSendToBack()
{
  --miMinZ;
  setZValue(miMinZ);
}

void MainWin::slotProperties()
{
  Node *poNode = selectedNode();
  Link *poLink = selectedLink();

  if (poNode) {
    DlgProperties oDialog(poNode, this);
    oDialog.exec();
  } else if (poLink) {
    QColor color = QColorDialog::getColor(poLink->color(), this);
    if (color.isValid())
      poLink->setColor(color);
  }
}

void MainWin::slotUpdateActions()
{
  bool hasSelection = !mpoScene->selectedItems().isEmpty();
  bool isNode = (selectedNode() != 0);
  bool isNodePair = (selectedNodePair() != NodePair_T());

  mpoActionCut->setEnabled(isNode);
  mpoActionCopy->setEnabled(isNode);
  mpoActionLink->setEnabled(isNodePair);
  mpoActionDelete->setEnabled(hasSelection);
  mpoActionBringToFront->setEnabled(isNode);
  mpoActionBringToBack->setEnabled(isNode);
  mpoActionProperties->setEnabled(isNode);

  foreach (QAction *action, mpoView->actions())
    mpoView->removeAction(action);

  foreach (QAction *action, mpoEditMenu->actions()) {
    if (action->isEnabled())
      mpoView->addAction(action);
  }
}

// new QGraphicsItem was selected
void MainWin::slotSelectionChanged()
{
  slotUpdateActions();

  if ( !mpoDlgAttributes ) return;


  DomViewIfc* poDomView = 0; // selected element

  poDomView = selectedNode();
  if ( !poDomView )
  {
    poDomView = selectedLink();
  }

  mpoDlgAttributes->setElement( poDomView );
}

// selection of definition changed
void MainWin::slotDefinitionSelectionChanged()
{
  if ( !mpoDefinitionNav ) return;
  if ( !mpoDlgAttributes ) return;

  QList<QTreeWidgetItem *> oSelectedItems = mpoDefinitionNav->selectedItems();
  if ( oSelectedItems.isEmpty()) return;

  DomViewIfc* poItem =
      FSMElementManager::getInstance().getDefinition( oSelectedItems.first());
  if ( poItem)
  {
    mpoDlgAttributes->setElement( poItem );
  }
}

void MainWin::createActions()
{
  mpoActionOpen = new QAction(tr("&Open..."), this);
  mpoActionOpen->setShortcuts(QKeySequence::Open);
  connect(mpoActionOpen, SIGNAL(triggered()), this, SLOT(slotOpen()));

  mpoActionSaveAs = new QAction(tr("&Save As..."), this);
  mpoActionSaveAs->setShortcuts(QKeySequence::SaveAs);
  connect(mpoActionSaveAs, SIGNAL(triggered()), this, SLOT(slotSaveAs()));

  mpoActionClose = new QAction(tr("&Close..."), this);
  mpoActionClose->setShortcuts(QKeySequence::Close);
  connect(mpoActionClose, SIGNAL(triggered()), this, SLOT(slotClose()));

  mpoActionExit = new QAction(tr("E&xit"), this);
  mpoActionExit->setShortcut(tr("Ctrl+Q"));
  connect(mpoActionExit, SIGNAL(triggered()), this, SLOT(close()));

  mpoActionNode = new QAction(tr("Add &Node"), this);
  mpoActionNode->setIcon(QIcon(":/images/node.png"));
  mpoActionNode->setShortcut(tr("Ctrl+N"));
  connect(mpoActionNode, SIGNAL(triggered()), this, SLOT(slotAddNode()));

  mpoActionLink = new QAction(tr("Add &Link"), this);
  mpoActionLink->setIcon(QIcon(":/images/link.png"));
  mpoActionLink->setShortcut(tr("Ctrl+L"));
  connect(mpoActionLink, SIGNAL(triggered()), this, SLOT(slotAddLink()));

  mpoActionDefinition = new QAction( tr( "Add &Definition"), this);
  mpoActionDefinition->setIconText( tr("D"));
  mpoActionDefinition->setShortcut( tr( "Ctrl+D"));
  connect( mpoActionDefinition, SIGNAL( triggered()), this, SLOT(slotAddDefinition()));

  mpoActionDelete = new QAction(tr("&Delete"), this);
  mpoActionDelete->setIcon(QIcon(":/images/delete.png"));
  mpoActionDelete->setShortcut(tr("Del"));
  connect(mpoActionDelete, SIGNAL(triggered()), this, SLOT(slotDelete()));

  mpoActionCut = new QAction(tr("Cu&t"), this);
  mpoActionCut->setIcon(QIcon(":/images/cut.png"));
  mpoActionCut->setShortcut(tr("Ctrl+X"));
  connect(mpoActionCut, SIGNAL(triggered()), this, SLOT(slotCut()));

  mpoActionCopy = new QAction(tr("&Copy"), this);
  mpoActionCopy->setIcon(QIcon(":/images/copy.png"));
  mpoActionCopy->setShortcut(tr("Ctrl+C"));
  connect(mpoActionCopy, SIGNAL(triggered()), this, SLOT(slotCopy()));

  mpoActionPaste = new QAction(tr("&Paste"), this);
  mpoActionPaste->setIcon(QIcon(":/images/paste.png"));
  mpoActionPaste->setShortcut(tr("Ctrl+V"));
  connect(mpoActionPaste, SIGNAL(triggered()), this, SLOT(slotPaste()));

  mpoActionBringToFront = new QAction(tr("Bring to &Front"), this);
  mpoActionBringToFront->setIcon(QIcon(":/images/bringtofront.png"));
  connect(mpoActionBringToFront, SIGNAL(triggered()),
          this, SLOT(slotBringToFront()));

  mpoActionBringToBack = new QAction(tr("&Send to Back"), this);
  mpoActionBringToBack->setIcon(QIcon(":/images/sendtoback.png"));
  connect(mpoActionBringToBack, SIGNAL(triggered()),
          this, SLOT(slotSendToBack()));

  mpoActionProperties = new QAction(tr("P&roperties..."), this);
  connect(mpoActionProperties, SIGNAL(triggered()),
          this, SLOT(slotProperties()));

  // actions for definitions
  mpoActionDefinitionsDelete = new QAction( tr("Delete"), this );

}

void MainWin::createMenus()
{
  mpoFileMenu = menuBar()->addMenu(tr("&File"));
  mpoFileMenu->addAction( mpoActionOpen);
  mpoFileMenu->addAction( mpoActionSaveAs);
  mpoFileMenu->addAction( mpoActionClose );
  mpoFileMenu->addAction( mpoActionExit);

  mpoEditMenu = menuBar()->addMenu(tr("&Edit"));
  mpoEditMenu->addAction(mpoActionNode);
  mpoEditMenu->addAction(mpoActionLink);
  mpoEditMenu->addAction( mpoActionDefinition);
  mpoEditMenu->addAction(mpoActionDelete);
  mpoEditMenu->addSeparator();
  mpoEditMenu->addAction(mpoActionCut);
  mpoEditMenu->addAction(mpoActionCopy);
  mpoEditMenu->addAction(mpoActionPaste);
  mpoEditMenu->addSeparator();
  mpoEditMenu->addAction(mpoActionBringToFront);
  mpoEditMenu->addAction(mpoActionBringToBack);
  mpoEditMenu->addSeparator();
  mpoEditMenu->addAction(mpoActionProperties);

  // view menu
  mpoViewMenu = menuBar()->addMenu(tr("&View"));
}

void MainWin::createToolBars()
{
  mpoEditToolBar = addToolBar(tr("Edit"));
  mpoEditToolBar->addAction(mpoActionNode);
  mpoEditToolBar->addAction(mpoActionLink);
  mpoEditToolBar->addAction( mpoActionDefinition);
  mpoEditToolBar->addAction(mpoActionDelete);
  mpoEditToolBar->addSeparator();
  mpoEditToolBar->addAction(mpoActionCut);
  mpoEditToolBar->addAction(mpoActionCopy);
  mpoEditToolBar->addAction(mpoActionPaste);
  mpoEditToolBar->addSeparator();
  mpoEditToolBar->addAction(mpoActionBringToFront);
  mpoEditToolBar->addAction(mpoActionBringToBack);
}

void MainWin::createNavigation()
{
  QDockWidget *poDockDefinitions = new QDockWidget(tr("Definitions"), this);
  poDockDefinitions->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  mpoDefinitionNav = new QTreeWidget(poDockDefinitions);
  mpoDefinitionNav->setContextMenuPolicy(Qt::ActionsContextMenu);
  mpoDefinitionNav->setColumnCount( 2 );
  // context menu for definitions
  mpoDefinitionNav->addAction( mpoActionDefinitionsDelete );
  connect( mpoDefinitionNav, SIGNAL( itemSelectionChanged() ), this, SLOT( slotDefinitionSelectionChanged() ) );
  connect( mpoActionDefinitionsDelete, SIGNAL( triggered() ), this, SLOT( slotDeleteDefinition() ) );
  poDockDefinitions->setWidget( mpoDefinitionNav );
  addDockWidget(Qt::LeftDockWidgetArea, poDockDefinitions);
  assert( mpoViewMenu);
  mpoViewMenu->addAction(poDockDefinitions->toggleViewAction());


  // Level view
  QDockWidget *poDockLevels = new QDockWidget(tr("Levels"), this);
  poDockLevels->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  mpoLevelNav = new QTreeWidget(poDockLevels);

  poDockLevels->setWidget( mpoLevelNav );
  addDockWidget(Qt::LeftDockWidgetArea, poDockLevels);
  assert( mpoViewMenu);
  mpoViewMenu->addAction(poDockLevels->toggleViewAction());

#if 0
  // States
  QDockWidget *poDockStates = new QDockWidget(tr("States"), this);
  poDockStates->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  mpoStateNav = new QTreeWidget(poDockStates);

  poDockStates->setWidget( mpoStateNav );
  addDockWidget(Qt::LeftDockWidgetArea, poDockStates);
  assert( mpoViewMenu);
  mpoViewMenu->addAction(poDockStates->toggleViewAction());
#endif

  // Attributes
  QDockWidget *poDockAttributes = new QDockWidget(tr("Attributes"), this);
  poDockAttributes->setAllowedAreas(Qt::BottomDockWidgetArea);
  poDockAttributes->setMinimumHeight( 150 );
  mpoDlgAttributes = new DlgAttributes(poDockAttributes);
  poDockAttributes->setWidget( mpoDlgAttributes );
  addDockWidget( Qt::BottomDockWidgetArea, poDockAttributes );
  assert( mpoViewMenu);
  mpoViewMenu->addAction(poDockAttributes->toggleViewAction());

}

void MainWin::setZValue(int z)
{
  Node *node = selectedNode();
  if (node)
    node->setZValue(z);
}

void MainWin::setupNode(Node *poInNode)
{
  poInNode->setPos(QPoint(80 + (100 * (miNumber % 5)),
                          80 + (50 * ((miNumber / 5) % 7))));
  mpoScene->addItem(poInNode);
  ++miNumber;

  mpoScene->clearSelection();
  poInNode->setSelected(true);
  slotBringToFront();
}

Node *MainWin::selectedNode() const
{
  QList<QGraphicsItem *> items = mpoScene->selectedItems();
  if (items.count() == 1) {
    return dynamic_cast<Node *>(items.first());
  } else {
    return 0;
  }
}

Link *MainWin::selectedLink() const
{
  QList<QGraphicsItem *> items = mpoScene->selectedItems();
  if (items.count() == 1) {
    return dynamic_cast<Link *>(items.first());
  } else {
    return 0;
  }
}

MainWin::NodePair_T MainWin::selectedNodePair() const
{
  QList<QGraphicsItem *> items = mpoScene->selectedItems();
  if (items.count() == 2) {
    Node *first = dynamic_cast<Node *>(items.first());
    Node *second = dynamic_cast<Node *>(items.last());
    if (first && second)
      return NodePair_T(first, second);
  }
  return NodePair_T();
}
