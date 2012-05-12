
add_executable(mjc mjc.cpp)
target_link_libraries(mjc ${LLVM_LIB} ${ANTLR_LIB} Parser Symbols Codegen)

