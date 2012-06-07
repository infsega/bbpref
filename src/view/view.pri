DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += \
  $$PWD/deskview.h \
  $$PWD/optiondialog.h \
  $$PWD/newgameform.h \
  $$PWD/formbid.h \
  $$PWD/scorewidget.h \
    src/view/scorehistory.h \
    src/view/aboutdialog.h

SOURCES += \
  $$PWD/deskview.cpp \
  $$PWD/formbid.cpp \
  $$PWD/optiondialog.cpp \
  $$PWD/scorewidget.cpp \
    src/view/scorehistory.cpp \
    src/view/aboutdialog.cpp \
    src/view/newgameform.cpp

FORMS += \
  $$PWD/optform.ui \
  $$PWD/newgameform.ui \
  $$PWD/aboutdialog.ui
