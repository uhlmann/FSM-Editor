/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef MAINWIN_H
#define MAINWIN_H

#include <QMainWindow>
#include <QPair>

class DlgAttributes;
class DlgDefinitionNew;
class Link;
class Node;

class QAction;
class QSpinBox;
class QDomElement;
class QGraphicsItem;
class QGraphicsScene;
class QGraphicsView;
class QTreeWidget;
class QTreeWidgetItem;

class MainWin : public QMainWindow
{
  Q_OBJECT

public:
  MainWin();

  inline QTreeWidget* getDefinitionView() const { return mpoDefinitionNav; }
  inline QTreeWidget* getLevelView() const      { return mpoLevelNav; }
  inline QTreeWidget* getStateView() const      { return mpoStateNav; }

public slots:
  void slotAddNode( const QDomElement * poInElement = 0 );
  void slotAddLink( const QDomElement * poInElement = 0 );
  void slotAddDefinition();
  void slotChangedLevel( QTreeWidgetItem*, QTreeWidgetItem* );

private slots:
  void slotOpen();
  void slotSaveAs();
  void slotClose();
  void slotDelete();
  void slotDeleteDefinition();
  void slotCut();
  void slotCopy();
  void slotPaste();
  void slotBringToFront();
  void slotSendToBack();
  void slotProperties();
  void slotUpdateActions();
  // new QGraphicsItem was selected
  void slotSelectionChanged();
  // selection of definition changed
  void slotDefinitionSelectionChanged();
  // zoom changed
  void slotZoomChanged( int );

private:
  typedef QPair<Node *, Node *> NodePair_T;

  void createSpinBoxes();
  void createActions();
  void createMenus();
  void createToolBars();
  void createNavigation();
  void setZValue(int z);
  void setupNode(Node *node);
  void setupEnter(Node *node);
  Node *selectedNode();
  Link *selectedLink() const;
  NodePair_T selectedNodePair() const;

  QMenu *mpoFileMenu;
  QMenu *mpoEditMenu;
  QMenu *mpoViewMenu; /// menu with views
  QSpinBox *mpoSBZoomPercent;
  QToolBar *mpoEditToolBar;
  QAction *mpoActionOpen;
  QAction *mpoActionSaveAs;
  QAction *mpoActionClose;
  QAction *mpoActionExit;
  QAction *mpoActionNode;
  QAction *mpoActionEntryNode;
  QAction *mpoActionLink;
  QAction *mpoActionDefinition;
  QAction *mpoActionDelete;
  QAction *mpoActionCut;
  QAction *mpoActionCopy;
  QAction *mpoActionPaste;
  QAction *mpoActionBringToFront;
  QAction *mpoActionBringToBack;
  QAction *mpoActionProperties;

  // action for defintion
  QAction *mpoActionDefinitionsDelete;

  QGraphicsScene *mpoScene;
  QGraphicsView *mpoView;
  QTreeWidget*     mpoDefinitionNav;     // navigation for definition
  QTreeWidget*     mpoLevelNav;          // navigation for level selection
  QTreeWidget*     mpoStateNav;          // state navigation
  DlgAttributes*   mpoDlgAttributes;     // Dialog to show dom attributes
  DlgDefinitionNew* mpoDlgDefinitionNew; // dialog for new definition
  Node* mpoSelectedNode;

  int miMinZ;
  int miMaxZ;
  int miNumber;
};

#endif
