
add_executable(ast-prettyprint ast-prettyprint.cpp)
target_link_libraries(ast-prettyprint ${ANTLR_LIB} Parser)

