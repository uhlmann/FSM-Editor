/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef FSMELEMENTMANAGER_H
#define FSMELEMENTMANAGER_H

#include <QDomDocument>
#include <QDomElement>
#include <QHash>
#include <QMap>
#include <QString>
#include <QObject>
#include <QIODevice>

/*
 forward
 */
class FSMElementIfc;
class FSMDefinitionIfc;
class MainWin;
class DomViewIfc;
class QTreeWidgetItem;

/*
 singleton implementation of manager for elements in FSMEditor
*/

class FSMElementManager : public QObject
{
    Q_OBJECT
public:

    typedef QHash<DomViewIfc*, QDomElement>      ElementToDomHash_T;
    typedef QHash<DomViewIfc*, QTreeWidgetItem*> ElementToTreeItem_T;
    typedef QMap<QString, FSMDefinitionIfc*>     IdToDefinitionMap_T;


    // possible xml elements
    enum XMLElements_T
    {
        XE_DEFINITIONS  // definitions
        , XE_FSM        // fsm description
        , XE_SCENE      // scene description
        , XE_ACTION     // action
        , XE_TRIGGER    // trigger
        , XE_TIMER      // timer
        , XE_STATE      // state
        , XE_TRANSITION // transition
        , XE_LAST       // end marker

    };

    enum XMLSections_T
    {
      XS_DEFINITIONS // section definitions
      , XS_FSM       // section fsm
      , XS_SCENE     // section scene
      , XS_LAST      // end marker
    };

    static const QString gmaXMLSectionNames[ XE_LAST ];

    // name of root element
    static const QString moRootName;

    virtual ~FSMElementManager();

    static FSMElementManager& getInstance();

    // return current level
    inline const QString& getLevel() const { return moLevel; }

    // add definition to be managed - returns true on success
    bool addDefinition( FSMDefinitionIfc*);
    // add element to be managed - returns true on success
    bool addElement( FSMElementIfc* );
    // add info to fsm section of dom
    void addDomFSMInfo( FSMElementIfc*);
    // add info to definition sectio of dom
    void addDomDefinitionInfo( FSMDefinitionIfc*);
    // add info to scene section of dom
    void addDomSceneInfo( FSMElementIfc* );
    // create tree item and add it to the navigation tree
    void addTreeItem( FSMElementIfc* );
    // create tree item and add it to the defintion tree
    void addTreeItem( FSMDefinitionIfc*);
    // register main window to position message boxes etc
    bool addMainWin( MainWin* );
    // return definition by id, or 0 if non existant
    FSMDefinitionIfc* getDefinition( const QString& ) const;
    // return definition by related tree item, or 0 if not related
    DomViewIfc* getDefinition( const QTreeWidgetItem* poInItem) const;
    // return item by id, or 0 if not existant
    FSMElementIfc* getItem( const QString& roInId) const;
    // returns registered dom node or 0, if not registered
    QDomElement getDomElement( XMLSections_T, DomViewIfc* );
    // actions to apply when id of an element changes
    void changedId
    (
        FSMElementIfc& roInOutElement
        , const QString& roInNewId
        , const QString& roInOldId
    );

    // actions to apply when id of an definitioin changes
    void changedId
    (
        FSMDefinitionIfc& roInOutElement
        , const QString& roInNewId
        , const QString& roInOldId
    );

    // read fsm defintion from an xml file
    bool read( QIODevice*);
    // write fsm definition as xml file
    bool write( QIODevice*);

    // enter new level - show / hide scene elements
    // if bInForce == false actions were only applied, if level
    // changes
    void setLevel( const QString& roInLevel, bool bInForce=false );
protected:
    explicit FSMElementManager(QObject *parent = 0);

    MainWin* mpoMainWindow; /// pointer to main window for message box positions

    // current level
    QString moLevel;

    // id to defintion object
    IdToDefinitionMap_T moDefinitions;

    // id to element
    QMap<QString, FSMElementIfc*> moElements;

    // DOM document to be written
    QDomDocument                        moDomDocumentWrite;
    // DOM document used for reading
    QDomDocument                        moDomDocumentRead;
    // root element of document
    QDomElement                         moRootElement;
    // section fsm
    QDomElement                         moRootFSMElement;
    // section defintion
    QDomElement                         moRootDefinitionElement;
    // section for scene information
    QDomElement                         moRootSceneElement;
    // root of tree element
    QTreeWidgetItem*                    mpoRootTreeItem;
    // root of tree for defintions
    QTreeWidgetItem*                    mpoRootTreeItemDefinitions;
    // hash to map element to dom element of related section
    ElementToDomHash_T moItemToDomElement[ XS_LAST ];
    // hash to map element to tree item
    ElementToTreeItem_T moItemToTreeItem;
    // hash to map definition to tree item
    ElementToTreeItem_T moDefinitionToTreeItem;
    // hash to map definition to tree item
    // parse section scene and create elements if the were not created
    bool parseSceneElement( const QDomElement& );
    // parse section definitions and create default scene elements if the were not created
    bool parseDefinitionsElement( const QDomElement& );
    // parse section fsm and create default scene elements if the were not created
    bool parseFSMElement( const QDomElement& );
    // parse state and create related graphic element
    bool parseStateElement( const QDomElement& );
    // parse transitions and create related graphic element
    bool parseTransitionElement( const QDomElement& );

private:
    // prevent automatic generation by compiler
    FSMElementManager( const FSMElementManager& );
    FSMElementManager& operator=( const FSMElementManager&);

    // element tags
    static const QString moDocumentName;
    // counter for definitions
    static unsigned int guiDefCnt;

signals:

public slots:

    void slotElementDestroyed( QObject* );
    void slotMainWinDestroyed();

};

#endif // FSMELEMENTMANAGER_H
