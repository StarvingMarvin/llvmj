cmake_minimum_required (VERSION 2.8)
project (llvmj)

configure_file (
  "${PROJECT_SOURCE_DIR}/llvmjConfig.h.in"
  "${PROJECT_BINARY_DIR}/llvmjConfig.h"
)
       
include_directories("${PROJECT_BINARY_DIR}" /usr/lib/llvm-2.9/include /usr/lib/llvm-2.8/include)

link_directories(/usr/lib/llvm /usr/lib/llvm-2.9/lib /usr/lib/x86_64-linux-gnu)

find_library(LLVM_LIB
                NAMES LLVM-2.9 LLVM LLVM-2.8 libLLVM-2.9 libLLVM libLLVM-2.8
                PATHS /usr/lib/llvm /usr/lib /usr/lib/llvm-2.9/lib /usr/lib/x86_64-linux-gnu)

add_subdirectory(example)

add_executable(llvmj llvmj.cpp mjmodule.cpp)

target_link_libraries(llvmj ${LLVM_LIB})


