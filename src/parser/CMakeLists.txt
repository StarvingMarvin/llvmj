
FIND_PROGRAM(ANTLR NAMES antlr3 antlr)

SET(PARS_SRC ${CMAKE_CURRENT_SOURCE_DIR})
SET(PARS_DST ${CMAKE_CURRENT_BINARY_DIR})

SET (ANTLR_OUTPUT ${PARS_DST}/MicroJavaLexer.h ${PARS_DST}/MicroJavaLexer.c 
    ${PARS_DST}/MicroJavaParser.c ${PARS_DST}/MicroJavaParser.h ${PARS_DST}/MicroJava.tokens)

ADD_CUSTOM_COMMAND(COMMAND ${ANTLR}
    ARGS -o ${PARS_DST} ${PARS_SRC}/MicroJava.g
    DEPENDS ${PARS_SRC}/MicroJava.g
    OUTPUT  ${ANTLR_OUTPUT})

SET_SOURCE_FILES_PROPERTIES(${ANTLR_OUTPUT} PROPERTIES GENERATED true)

#ADD_CUSTOM_COMMAND(COMMAND ${ANTLR}
                    #ARGS -o ${DST} -l ${DST} ${SRC}/MicroJavaTree.g
                    #DEPENDS ${SRC}/MicroJavaTree.g ${DST}/MicroJava.tokens
                    #OUTPUT ${DST}/MicroJavaTree.h ${DST}/MicroJavaTree.c)

ADD_LIBRARY(Parser parser.cpp semantics.cpp ${PARS_DST}/MicroJavaLexer.c ${PARS_DST}/MicroJavaParser.c)


