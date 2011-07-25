include($$PWD/src/main.pri)

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
RESOURCES += $$PWD/openpref.qrc
TRANSLATIONS = openpref_ru.ts

## Build qm files ##

isEmpty(QMAKE_LRELEASE) {
  win32:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]\lrelease.exe
  else:QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

updateqm.input = TRANSLATIONS
updateqm.output = ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN} -qm ${QMAKE_FILE_PATH}/${QMAKE_FILE_BASE}.qm
updateqm.CONFIG += no_link target_predeps

QMAKE_EXTRA_COMPILERS += updateqm
PRE_TARGETDEPS += compiler_updateqm_make_all

## Handle installation ##

isEmpty(BIN_INSTALL_DIR) {
  BIN_INSTALL_DIR = games
}

isEmpty(PREFIX) {
  PREFIX = /usr/local
}

target.path = $$PREFIX/$$BIN_INSTALL_DIR
INSTALLS += target
