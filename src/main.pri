DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += \
  $$PWD/kpref.h \
  $$PWD/prfconst.h

SOURCES += \
  $$PWD/kpref.cpp \
  $$PWD/prfconst.cpp \
  $$PWD/main.cpp

include($$PWD/view/view.pri)
include($$PWD/logic/logic.pri)
include($$PWD/model/model.pri)
include($$PWD/misc/misc.pri)
