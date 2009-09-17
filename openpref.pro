TEMPLATE = app
TARGET = openpref

QT += gui

CONFIG += qt warn_on
CONFIG += debug_and_release

QMAKE_CXXFLAGS += -DLOCAL

#QMAKE_CFLAGS_RELEASE ~= s/\-O./-Os
#QMAKE_CXXFLAGS_RELEASE ~= s/\-O./-Os

##QMAKE_CFLAGS_RELEASE ~= s/\-O./-O2
##QMAKE_CXXFLAGS_RELEASE ~= s/\-O./-O2

#QMAKE_CFLAGS_RELEASE += -march=native
#QMAKE_CXXFLAGS_RELEASE += -march=native
#QMAKE_CFLAGS_RELEASE += -mtune=native
#QMAKE_CXXFLAGS_RELEASE += -mtune=native

QMAKE_LFLAGS_RELEASE += -s


DESTDIR = .
OBJECTS_DIR = _build/obj
UI_DIR = _build/uic
MOC_DIR = _build/moc
RCC_DIR = _build/rcc

#include($$PWD/src/qtstatemachine/src/qtstatemachine.pri)
include($$PWD/src/main.pri)

RESOURCES += $$PWD/gfx.qrc

TRANSLATIONS = openpref_ru.ts
