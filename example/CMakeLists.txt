
#set(CMAKE_LLVM_SOURCE_FILE_EXTENSIONS "ll")
#set(CMAKE_LLVM_COMPILER "llvm-as")
#set(CMAKE_LLVM_COMPILER_ENV_VAR "")
#set(CMAKE_LLVM_OUTPUT_EXTENSION "bc")
#set(CMAKE_LLVM_LINKER_EXECUTABLE "llvm-link")
#set(CMAKE_LLVM_SOURCE_FILE_EXTENSIONS "ll")

#enable_language(LLVM)

#add_library(example example.ll)

execute_process(COMMAND echo llvm-as -o example.bc ${CMAKE_CURRENT_SOURCE_DIR}/example.ll
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
                OUTPUT_FILE mrt)

execute_process(COMMAND llvm-as -o example.bc ${CMAKE_CURRENT_SOURCE_DIR}/example.ll
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

execute_process(COMMAND chmod +x example.bc
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})
