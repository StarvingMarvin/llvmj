
add_subdirectory(parser)
add_subdirectory(symbols)
add_subdirectory(codegen)

add_executable(llvmj llvmj.cpp mjmodule.cpp)
target_link_libraries(llvmj ${LLVM_LIB})

