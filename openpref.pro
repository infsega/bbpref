include($$PWD/src/main.pri)

TEMPLATE = app
TARGET = openpref
QT += gui
CONFIG += qt warn_on
CONFIG += debug_and_release

QMAKE_LFLAGS_RELEASE += -s

DESTDIR = .
OBJECTS_DIR = _build/obj
UI_DIR = _build/uic
MOC_DIR = _build/moc
RCC_DIR = _build/rcc
RESOURCES += $$PWD/bbpref.qrc
TRANSLATIONS = bbpref_ru.ts
LIBS += c:/bbndk-2.0.1/target/qnx6/armle-v7/usr/lib/libbps.so

## Build qm files ##

isEmpty(QMAKE_LRELEASE) {
  QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
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

OTHER_FILES += \
    splashscreen.png \
    icon.png \
    bar-descriptor.xml
