
ADD_CUSTOM_COMMAND(COMMAND pdflatex
                    ARGS --output-directory=${CMAKE_CURRENT_BINARY_DIR} --draftmode ${CMAKE_CURRENT_SOURCE_DIR}/rad.tex
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/rad.tex
                            ${CMAKE_CURRENT_SOURCE_DIR}/titlepage.tex
                            ${CMAKE_CURRENT_SOURCE_DIR}/intro.tex
                            ${CMAKE_CURRENT_SOURCE_DIR}/antlr.tex
                            ${CMAKE_CURRENT_SOURCE_DIR}/llvm.tex
                            ${CMAKE_CURRENT_SOURCE_DIR}/abstract.tex
                            ${CMAKE_CURRENT_SOURCE_DIR}/bib.tex
                            ${CMAKE_CURRENT_SOURCE_DIR}/appendix.tex
                            ${CMAKE_CURRENT_SOURCE_DIR}/microjava.tex
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/rad.aux
                            ${CMAKE_CURRENT_BINARY_DIR}/rad.log
                            ${CMAKE_CURRENT_BINARY_DIR}/rad.toc)

ADD_CUSTOM_COMMAND(COMMAND pdflatex
                    ARGS --output-directory=${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/rad.tex
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/rad.toc
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    OUTPUT  ${CMAKE_CURRENT_BINARY_DIR}/rad.pdf)

ADD_CUSTOM_TARGET(Thesis ALL
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/rad.pdf)
