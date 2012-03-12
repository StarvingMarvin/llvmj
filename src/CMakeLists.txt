
add_subdirectory(parser)
add_subdirectory(symbols)
add_subdirectory(codegen)

add_executable(llvmj llvmj.cpp)
target_link_libraries(llvmj ${LLVM_LIB})

add_executable(mjc mjc.cpp)
target_link_libraries(mjc ${LLVM_LIB} ${ANTLR_LIB} Parser Symbols Codegen)

