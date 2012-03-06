
include_directories("${PROJECT_BINARY_DIR}/src/parser")
add_executable(ast-prettyprint ast-prettyprint.cpp)
target_link_libraries(ast-prettyprint ${ANTLR_LIB} Parser)

