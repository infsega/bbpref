DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += \
  $$PWD/deskview.h \
  $$PWD/optiondialog.h \
  $$PWD/newgameform.h \
  $$PWD/formbid.h \
  $$PWD/scorewidget.h \
    src/view/aboutdialog.h \
    src/view/gamelogdialog.h \
    src/view/savegamedialog.h

SOURCES += \
  $$PWD/deskview.cpp \
  $$PWD/formbid.cpp \
  $$PWD/optiondialog.cpp \
  $$PWD/scorewidget.cpp \
    src/view/aboutdialog.cpp \
    src/view/newgameform.cpp \
    src/view/gamelogdialog.cpp \
    src/view/savegamedialog.cpp

FORMS += \
  $$PWD/optform.ui \
  $$PWD/newgameform.ui \
  $$PWD/aboutdialog.ui \
    src/view/gamelogdialog.ui \
    src/view/savegamedialog.ui
