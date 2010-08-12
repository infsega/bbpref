TEMPLATE = app
TARGET = openpref

QT += gui

CONFIG += qt warn_on
CONFIG += debug_and_release

!isEmpty(USE_CONAN) {
    INCLUDEPATH += /usr/local/include/conan/include
    LIBS += -L/usr/local/lib64 -lConan
}

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

RESOURCES += $$PWD/openpref.qrc

TRANSLATIONS = openpref_ru.ts


isEmpty(BIN_INSTALL_DIR) {
  BIN_INSTALL_DIR = games
}

isEmpty(PREFIX) {
  PREFIX = /usr/local
}

target.path = $$PREFIX/$$BIN_INSTALL_DIR
INSTALLS += target
