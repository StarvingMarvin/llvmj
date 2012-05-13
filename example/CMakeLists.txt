
FIND_PROGRAM(LLVM_AS NAMES llvm-as llvm-as-3.0 llvm-as-2.9)

ADD_CUSTOM_COMMAND(COMMAND ${LLVM_AS}
                    ARGS -o example.bc ${CMAKE_CURRENT_SOURCE_DIR}/example.ll
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/example.ll
                    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example.bc)

ADD_CUSTOM_COMMAND(COMMAND chmod
                    ARGS +x example.bc
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/example.bc
                    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example.bcx)

ADD_CUSTOM_TARGET(MJExample ALL echo
   DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/example.bcx)

