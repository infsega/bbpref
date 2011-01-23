TEMPLATE = app
TARGET = openpref

QT += gui network

CONFIG += qt warn_on
CONFIG += debug_and_release
QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.4u.sdk
QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.4

# Universal binary
mac:release {
#  CONFIG += x86 ppc
}

mac {
  ICON = pics/openpref.icns
  TARGET = OpenPref
  QMAKE_INFO_PLIST = Info_mac.plist
}

!isEmpty(USE_CONAN) {
    INCLUDEPATH += /usr/local/include/conan/include
    LIBS += -L/usr/local/lib64 -lConan
}

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
