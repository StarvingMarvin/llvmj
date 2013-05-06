
add_executable(ast-prettyprint ast-prettyprint.cpp)
target_link_libraries(ast-prettyprint Parser ${ANTLR_LIB})

