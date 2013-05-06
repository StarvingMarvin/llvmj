
add_executable(mjc mjc.cpp)
target_link_libraries(mjc Parser MjSemantics Codegen ${LLVM_LIB} ${ANTLR_LIB})

