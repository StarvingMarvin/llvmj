
#add_executable(test test.cpp)
#target_link_libraries(test ${LLVM_LIB})

set(BIN ${CMAKE_CURRENT_BINARY_DIR})

include_directories("${PROJECT_BINARY_DIR}/src/parser")
add_executable(test-parser parser.cpp)
target_link_libraries(test-parser ${ANTLR_LIB} Parser)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/test.mj 
    DESTINATION ${BIN})

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_executable(test-symbol symbols.cpp)
target_link_libraries(test-symbol ${LLVM_LIB} Symbol)

enable_testing()

add_test(parser ${BIN}/test-parser ${BIN}/test.mj)
add_test(symbols ${BIN}/test-symbol)
#add_test(parsertest ) 

