/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/
#ifndef FSMELEMENTBASE_H
#define FSMELEMENTBASE_H

#include <QMap>
#include <QObject>
#include "elements/fsmelementifc.h"

class FSMElementBase : public QObject, public FSMElementIfc
{
    Q_OBJECT
public:
    explicit FSMElementBase(FSMElementIfc* poInElementParent = 0,QObject *parent = 0, const QString& roInPrototypeName = "", bool bInIsPrototype = false );

    virtual ~FSMElementBase();

    // pointer to parent element, if exists
    inline FSMElementIfc* getElementParent() const { return mpoElementParent; }
    // pointer to signal slot base class QObject
    inline QObject* getSignalSlotBase() const { return const_cast<QObject*>( dynamic_cast<const QObject*>( this ) ); }
    // return id
    inline const QString& getId() const  { return moId;    }
    // return id of element related to dom parent, if exists - or "" for root nodes
    inline const QString& getDomParentId() const { return moDomParentId; }
    // access to rleated dom element
    QDomElement getDomElement() const;
    // default implementation returning 0 to be overwritten in derived classes
    QDomElement createDomElement( QDomDocument& roInOutDomDocument ) const;
    // create dom element for scene information
    QDomElement createDomSceneElement( QDomDocument& roInOutDoDocuemt ) const;
    // create item to be added to a QTreeWidget or returns 0 if not supported by object
    QTreeWidgetItem* createTreeItem
    (
       const QDomElement& roInDomElement
       , QTreeWidgetItem *poInParentItem = 0
       , QTreeWidget*       poInView       = 0
    ) const;

    // assign attribute - returning false if not assigned
    bool setAttribute( const QString& roInName, const QString& roInValue);

    // apply attributes
    void applyAttributes( const QDomElement& roInElement );
    void applySceneAttributes( const QDomElement& roInElement );

    // returns string list with definition names
    static const QStringList getElementNames( void );

    // return the Id String of a dom element
    static QString getId( const QDomElement& roInElement );

    // array with tag names for xml representation
    static const QString gmaFSMElementTagName[ ET_LAST ];

protected:
    typedef QMap<QString, FSMElementBase* > TagNameToElementMap_T;

    // map with prototypes (not allocated with new!)
    static TagNameToElementMap_T& getPrototypes();

    // update dom attributes - to be overwritten in derived classes
    virtual void updateAttributes( QDomElement& roInOutElement ) const;
    // assign new scene attributes apply derived methods
    virtual void updateAttributesScene( QDomElement& roInOutElement) const;
    // method to change id
    void changedId( const QString& roInId );
    // assign id of element containing related dom parent
    inline void setDomParentId( const QString& roInId ) { moDomParentId = roInId; }
    // calculate id from a dom element - to be implemented in concrete derived class
    virtual QString calculateId( const QDomElement& roInElement) const;

    FSMElementIfc* mpoElementParent;

    QString      moId;          /// id of element
    QString      moDomParentId; /// Element Id of dom parent, no parent in scene
private:
    // prevent automatic creation
    FSMElementBase( const FSMElementBase& );
    FSMElementBase& operator=( const FSMElementBase&);

signals:
    // signal to inform about id change
    void sigChangedId( const FSMElementIfc& );

public slots:
    // slot to connect on signal sigChangedId( const FSMElementIfc&)
    virtual void slotUpdateId( const FSMElementIfc& );
};

#endif // FSMELEMENTBASE_H
