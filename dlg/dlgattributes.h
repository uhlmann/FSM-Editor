/*
(c) 2011 Matthias Uhlmann
         matthias.uhlmann@gmx.de
*/

#ifndef DLGATTRIBUTES_H
#define DLGATTRIBUTES_H

#include <QWidget>

#include "ui_dlgattributes.h"
#include "domviewifc.h"

class QDomElement;
class DomViewIfc;

class DlgAttributes : public QWidget, private Ui::DlgAttributes
{
    Q_OBJECT
public:
    explicit DlgAttributes(QWidget *poInParent = 0);

    // virtual destructor
    virtual ~DlgAttributes();

    /// assosiate new element
    void setElement( DomViewIfc* poInElement);

protected:
    // clear Table and set dimesion to (0,0)
    void clear();
    // actions to be applied when element changes
    void changedElement( DomViewIfc* poInElement );
    // actions to be applied when new element is set
    void assignElement( DomViewIfc* poInElement );
    // actions to be applied when element is released
    void releaseElement( DomViewIfc* poInElement );
    // display dom elements
    void setDomElement( const QDomElement& roInElement );

/*
 attributes
*/
    DomViewIfc* mpoElement; /// pointer to assosiated element

    bool           mbFilled;   /// table is filled

signals:

public slots:

    // slot to handle change of attributes in table
    void slotItemChanged ( QTableWidgetItem * item );
    // slot to handle destruction of assosiated element
    void slotDestroyed( QObject* );
    // slot to accept data
    void accept();
    // slot to reject data
    void reject();
};

#endif // DLGATTRIBUTES_H
