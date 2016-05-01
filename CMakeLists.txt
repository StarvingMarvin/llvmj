cmake_minimum_required (VERSION 2.8)
project (llvmj)

configure_file (
    "${PROJECT_SOURCE_DIR}/llvmjConfig.h.in"
    "${PROJECT_BINARY_DIR}/llvmjConfig.h"
    )

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Werror -std=c++11")

include_directories("${PROJECT_BINARY_DIR}" 
                    "${PROJECT_BINARY_DIR}/src"  
                    src
                    /usr/lib/llvm-3.6/include
                    /usr/lib/llvm/include)

link_directories(/usr/lib
                /usr/lib/llvm-3.6/lib
                /usr/lib/llvm/
                /usr/lib/x86_64-linux-gnu 
                "${PROJECT_BINARY_DIR}/src"
                "${PROJECT_BINARY_DIR}/src/codegen"
                "${PROJECT_BINARY_DIR}/src/symbols"
                "${PROJECT_BINARY_DIR}/src/parser")

find_library(LLVM_LIB
    NAMES LLVM-3.6 LLVM libLLVM
    PATHS /usr/lib/llvm-3.6/lib /usr/lib/x86_64-linux-gnu /usr/lib/llvm /usr/lib )

find_library(ANTLR_LIB
    NAMES libantlr3c antlr3c libantlr3 antlr3
    PATHS /usr/lib /usr/lib/antlr3 /usr/lib/antlr /usr/lib/x86_64-linux-gnu)

add_subdirectory(src)
add_subdirectory(example)
add_subdirectory(thesis EXCLUDE_FROM_ALL)
add_subdirectory(test)
