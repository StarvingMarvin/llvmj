
ADD_CUSTOM_COMMAND(COMMAND llvm-as
                    ARGS -o example.bc ${CMAKE_CURRENT_SOURCE_DIR}/example.ll
                    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/example.ll
                    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example.bc)

ADD_CUSTOM_COMMAND(COMMAND chmod
                    ARGS +x example.bc
                    DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/example.bc
                    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/example.bcx)

ADD_CUSTOM_TARGET(MJExample ALL echo
   DEPENDS   ${CMAKE_CURRENT_BINARY_DIR}/example.bcx)
