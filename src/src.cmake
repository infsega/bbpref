project(openpref) 

INCLUDE_DIRECTORIES( src )

SET ( HEADERS ${HEADERS}
  src/kpref.h 
  src/prfconst.h 
  src/main.h
)

SET ( SOURCES ${SOURCES}
  src/kpref.cpp 
  src/prfconst.cpp 
  src/main.cpp
)

SET ( UIS ${UIS}
  src/ui/optform.ui 
  src/ui/newgameform.ui 
  src/ui/helpbrowser.ui
)

# tell cmake to process CMakeLists.txt in that subdirectory
#add_subdirectory(util)
#add_subdirectory(cardutil)
#add_subdirectory(forms)
#add_subdirectory(score)
#add_subdirectory(logic)
#add_subdirectory(desk)

include(src/util/util.cmake)
include(src/cardutil/cardutil.cmake)
include(src/forms/forms.cmake)
include(src/score/score.cmake)
include(src/logic/logic.cmake)
include(src/desk/desk.cmake)

INCLUDE_DIRECTORIES( src )
INCLUDE_DIRECTORIES( src/util )
INCLUDE_DIRECTORIES( src/cardutil )
INCLUDE_DIRECTORIES( src/forms )
INCLUDE_DIRECTORIES( src/score )
INCLUDE_DIRECTORIES( src/logic )
INCLUDE_DIRECTORIES( src/desk )
