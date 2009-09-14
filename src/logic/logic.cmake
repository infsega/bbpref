project(openpref)

SET ( HEADERS ${HEADERS}
  src/logic/player.h 
  src/logic/aiplayer.h 
  src/logic/human.h 
  src/logic/aialphabeta.h
)

SET ( SOURCES ${SOURCES}
  src/logic/player.cpp 
  src/logic/human.cpp 
  src/logic/aiplayer.cpp 
  src/logic/aialphabeta.cpp
)