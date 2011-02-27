TEMPLATE      = app
HEADERS       = mainwin.h \
                fsmelementmanager.h \
                definitions/fsmdefinitionifc.h \
                elements/link.h \
                elements/node.h \
                elements/port.h \
                elements/fsmelementifc.h \
                elements/fsmelementbase.h \
                dlg/dlgproperties.h \
                dlg/dlgattributes.h \
                definitions/fsmdefinitionbase.h \
                definitions/fsmtrigger.h \
                definitions/fsmtimer.h \
                definitions/fsmevent.h \
                definitions/fsmmember.h \
                definitions/fsminclude.h \
                definitions/fsmaction.h \
    dlg/dlgdefinitonnew.h \
    domviewifc.h
   
SOURCES       = mainwin.cpp \
                main.cpp \
                fsmelementmanager.cpp \
                definitions/fsmdefinitionifc.cpp \
                elements/link.cpp \
                elements/node.cpp \                
                elements/port.cpp \
                elements/fsmelementifc.cpp \
                elements/fsmelementbase.cpp \
                dlg/dlgproperties.cpp \ 
                dlg/dlgattributes.cpp \
                definitions/fsmdefinitionbase.cpp \
                definitions/fsmtrigger.cpp \
                definitions/fsmtimer.cpp \
                definitions/fsmevent.cpp \
                definitions/fsmmember.cpp \
                definitions/fsminclude.cpp \
                definitions/fsmaction.cpp \
    dlg/dlgdefinitonnew.cpp
    
FORMS         = dlg/dlgproperties.ui \
                dlg/dlgattributes.ui \
                dlg/dlgdefinitionnew.ui

RESOURCES     = resources.qrc

INCLUDES     += definitions
INCLUDES     += elements

QT           += xml
