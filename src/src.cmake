project(openpref) 

INCLUDE_DIRECTORIES( src )
INCLUDE_DIRECTORIES( src/logic )
INCLUDE_DIRECTORIES( src/model )
INCLUDE_DIRECTORIES( src/view )
INCLUDE_DIRECTORIES( src/misc )

FILE(GLOB main_SRCS "src/*.cpp")
FILE(GLOB logic_SRCS "src/logic/*.cpp")
FILE(GLOB model_SRCS "src/model/*.cpp")
FILE(GLOB view_SRCS "src/view/*.cpp")
FILE(GLOB misc_SRCS "src/misc/*.cpp")

FILE(GLOB main_HDRS "src/*.h")
FILE(GLOB logic_HDRS "src/logic/*.h")
FILE(GLOB model_HDRS "src/model/*.h")
FILE(GLOB view_HDRS "src/view/*.h")
FILE(GLOB misc_HDRS "src/misc/*.h")

FILE(GLOB UIS "src/view/*.ui")

SET ( HEADERS
 ${main_HDRS}
 ${logic_HDRS}
 ${model_HDRS}
 ${view_HDRS}
 ${misc_HDRS}
)

SET ( SOURCES
 ${main_SRCS}
 ${logic_SRCS} 
 ${model_SRCS}
 ${view_SRCS}
 ${misc_SRCS}
)

#message("${SOURCES}")
