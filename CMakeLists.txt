project(openpref)

include(src/src.cmake)
include(CPack)

CMAKE_MINIMUM_REQUIRED( VERSION 2.4.0 )
SET( CMAKE_COLOR_MAKEFILE ON )
SET( CMAKE_VERBOSE_MAKEFILE ON )
SET( CMAKE_INCLUDE_CURRENT_DIR TRUE )
SET( CMAKE_MODULE_PATH ${openpref_SOURCE_DIR}/cmake )

IF( APPLE )
    SET( PROGNAME OpenPref )
    SET( MACOSX_BUNDLE_ICON_FILE OpenPref.icns )
    SET( MACOSX_BUNDLE_SHORT_VERSION_STRING 0.1.3 )
    SET( MACOSX_BUNDLE_VERSION 0.1.3 )
    SET( MACOSX_BUNDLE_LONG_VERSION_STRING Version 0.1.3 )
ELSE( APPLE )
    SET( PROGNAME openpref )
ENDIF( APPLE )

if (WIN32)
  add_definitions( -DWIN32 )
endif (WIN32)

IF( MINGW )
    # resource compilation for MinGW
    ADD_CUSTOM_COMMAND( OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/openprefres.o
                        COMMAND windres.exe -I${CMAKE_CURRENT_SOURCE_DIR} -i${CMAKE_CURRENT_SOURCE_DIR}/openprefres.rc 
                             -o ${CMAKE_CURRENT_BINARY_DIR}/openprefres.o )
    SET( SOURCES ${SOURCES} ${CMAKE_CURRENT_BINARY_DIR}/openprefres.o)
ELSE( MINGW )
#    SET( SOURCES ${SOURCES} openpref.rc)
ENDIF( MINGW )

SET ( RCS
	gfx.qrc
)

SET ( QM_SRCS 
	openpref_ru.ts
)

FIND_PACKAGE( Qt4 REQUIRED )
find_package(Linguist REQUIRED)

INCLUDE( ${QT_USE_FILE} )
QT4_ADD_RESOURCES( RC_SRCS ${RCS} )
QT4_WRAP_UI( UI_HEADERS ${UIS})
QT4_WRAP_CPP( MOC_SRCS ${HEADERS} )
QT4_WRAP_TS( QMS ${QM_SRCS} )


INCLUDE_DIRECTORIES( ${CMAKE_BINARY_DIR} )
ADD_EXECUTABLE( openpref ${SOURCES} ${MOC_SRCS} ${RC_SRCS} ${UI_HEADERS} ${QMS} )

TARGET_LINK_LIBRARIES( openpref ${QT_LIBRARIES} )

INSTALL (TARGETS openpref DESTINATION "games")
install(FILES ${QMS} DESTINATION "share/openpref/i18n"
)

# Add support for an uninstall target
configure_file(
  "${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  IMMEDIATE @ONLY)

add_custom_target(uninstall
  "${CMAKE_COMMAND}" -P
  "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
  
)