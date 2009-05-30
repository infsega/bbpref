DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

HEADERS += \
  $$PWD/kpref.h \
  $$PWD/prfconst.h \
  $$PWD/optform.h \
  $$PWD/main.h

SOURCES += \
  $$PWD/kpref.cpp \
  $$PWD/prfconst.cpp \
  $$PWD/optform.cpp \
  $$PWD/main.cpp

FORMS += \
  $$PWD/ui/optform.ui

include($$PWD/util/util.pri)
include($$PWD/cardutil/cardutil.pri)
include($$PWD/forms/forms.pri)
include($$PWD/score/score.pri)
include($$PWD/logic/logic.pri)
include($$PWD/desk/desk.pri)
