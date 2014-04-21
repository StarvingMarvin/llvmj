
add_executable(mjc mjc.cpp ezOptionParser.hpp)
target_link_libraries(mjc Codegen Parser MjSemantics ${LLVM_LIB} ${ANTLR_LIB})

add_executable(mji mji.cpp ezOptionParser.hpp)
target_link_libraries(mji Codegen Parser MjSemantics ${LLVM_LIB} ${ANTLR_LIB})
