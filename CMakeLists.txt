cmake_minimum_required(VERSION 2.8)
project( ocfdfsmeditor CXX C)

#INCLUDE (${CMAKE_ROOT}/Modules/FindQt.cmake)
find_package( Qt4 COMPONENTS QtCore QtGui QtXml REQUIRED )



INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_SOURCE_DIR} )
INCLUDE_DIRECTORIES( ${CMAKE_CURRENT_BINARY_DIR} )
INCLUDE_DIRECTORIES( definitions )
INCLUDE_DIRECTORIES( elements )

set (QT_USE_XML 1)

set (SOURCES
  mainwin.cpp
  main.cpp
  fsmelementmanager.cpp
  definitions/fsmdefinitionifc.cpp
  elements/link.cpp
  elements/node.cpp
  elements/port.cpp
  elements/fsmelementifc.cpp
  elements/fsmelementbase.cpp
  dlg/dlgproperties.cpp
  dlg/dlgattributes.cpp
  definitions/fsmdefinitionbase.cpp
  definitions/fsmtrigger.cpp
  definitions/fsmtimer.cpp
  definitions/fsmevent.cpp
  definitions/fsmmember.cpp
  definitions/fsminclude.cpp
  definitions/fsmaction.cpp
  dlg/dlgdefinitonnew.cpp
)

QT4_ADD_RESOURCES( OCFDFSMEDITOR_RC_SRCS resources.qrc )


QT4_WRAP_UI( OCFDFSMEDITOR_UI_FILES
  dlg/dlgproperties.ui
  dlg/dlgattributes.ui
  dlg/dlgdefinitionnew.ui
)

QT4_WRAP_CPP( OCFDFSMEDITOR_MOC_HEADERS
  mainwin.h
  definitions/fsminclude.h
  elements/fsmelementbase.h
  fsmelementmanager.h
  dlg/dlgattributes.h
  definitions/fsmdefinitionbase.h
  dlg/dlgproperties.h
  dlg/dlgattributes.h
  dlg/dlgdefinitonnew.h
)

include(${QT_USE_FILE})
INCLUDE_DIRECTORIES( ${QT_INCLUDE_DIR} )
add_executable(ocfdfsmeditor ${SOURCES} ${OCFDFSMEDITOR_UI_FILES} ${OCFDFSMEDITOR_RC_SRCS} ${OCFDFSMEDITOR_MOC_HEADERS})
target_link_libraries(ocfdfsmeditor ${QT_LIBRARIES} )

