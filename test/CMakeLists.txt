
add_executable(test test.cpp)
target_link_libraries(test ${LLVM_LIB})

include_directories ("${PROJECT_SOURCE_DIR}/src")
add_executable(symbolstest symbols.cpp)
target_link_libraries(symbolstest ${LLVM_LIB} Symbol)

enable_testing()

add_test(symbols ${CMAKE_CURRENT_BINARY_DIR}/symbolstest)

