project(openpref) 

INCLUDE_DIRECTORIES( src )
INCLUDE_DIRECTORIES( src/logic )
INCLUDE_DIRECTORIES( src/model )
INCLUDE_DIRECTORIES( src/view )

FILE(GLOB main_SRCS "src/*.cpp")
FILE(GLOB logic_SRCS "src/logic/*.cpp")
FILE(GLOB model_SRCS "src/model/*.cpp")
FILE(GLOB view_SRCS "src/view/*.cpp")

FILE(GLOB main_HDRS "src/*.h")
FILE(GLOB logic_HDRS "src/logic/*.h")
FILE(GLOB model_HDRS "src/model/*.h")
FILE(GLOB view_HDRS "src/view/*.h")

FILE(GLOB UIS "src/view/*.ui")

SET ( HEADERS ${main_HDRS} ${logic_HDRS} ${model_HDRS} ${view_HDRS})
SET ( SOURCES ${main_SRCS} ${logic_SRCS} ${model_SRCS} ${view_SRCS})

#message("${SOURCES}")
