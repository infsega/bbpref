DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += \
  $$PWD/deskview.h \
  $$PWD/optiondialog.h \
  $$PWD/playersinfodialog.h \
  $$PWD/newgameform.h \
  $$PWD/formbid.h \
  $$PWD/scorewidget.h \
    src/view/scorehistory.h

SOURCES += \
  $$PWD/deskview.cpp \
  $$PWD/formbid.cpp \
  $$PWD/optiondialog.cpp \
  $$PWD/playersinfodialog.cpp \
  $$PWD/scorewidget.cpp \
    src/view/scorehistory.cpp

FORMS += \
  $$PWD/optform.ui \
  $$PWD/newgameform.ui
