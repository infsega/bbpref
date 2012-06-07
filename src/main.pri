DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += \
  $$PWD/prfconst.h \
    src/mainwindow.h

SOURCES += \
  $$PWD/prfconst.cpp \
  $$PWD/main.cpp \
    src/mainwindow.cpp

include($$PWD/view/view.pri)
include($$PWD/logic/logic.pri)
include($$PWD/model/model.pri)

FORMS += \
    src/mainwindow.ui
