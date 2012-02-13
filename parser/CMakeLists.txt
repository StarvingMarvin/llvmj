
FIND_PROGRAM(ANTLR NAMES antlr3 antlr)

SET(SRC ${CMAKE_CURRENT_SOURCE_DIR})
SET(DST ${CMAKE_CURRENT_BINARY_DIR})

ADD_CUSTOM_COMMAND(COMMAND ${ANTLR}
                    ARGS -o ${DST} ${SRC}/MicroJava.g
                    DEPENDS ${SRC}/MicroJava.g
                    OUTPUT  ${DST}/MicroJavaLexer.h ${DST}/MicroJavaLexer.c 
                            ${DST}/MicroJavaParser.c ${DST}/MicroJavaParser.h ${DST}/MicroJava.tokens)

ADD_CUSTOM_COMMAND(COMMAND ${ANTLR}
                    ARGS -o ${DST} -l ${DST} ${SRC}/MicroJavaTree.g
                    DEPENDS ${SRC}/MicroJavaTree.g ${DST}/MicroJava.tokens
                    OUTPUT ${DST}/MicroJavaTree.h ${DST}/MicroJavaTree.c)

#ADD_CUSTOM_COMMAND(COMMAND chmod
#                    ARGS +x example.bc
#                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/example.bc
#                    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example.bcx)

#ADD_CUSTOM_TARGET(Grammar ALL echo
#   DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/example.bcx)

INCLUDE_DIRECTORIES(${DST})

ADD_LIBRARY(parser ${DST}/MicroJavaLexer.c ${DST}/MicroJavaParser.c) #${DST}/MicroJavaTree.c

