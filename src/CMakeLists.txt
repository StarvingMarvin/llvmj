
link_directories(/usr/lib 
                /usr/lib/x86_64-linux-gnu 
                "${PROJECT_BINARY_DIR}/src"
                "${PROJECT_BINARY_DIR}/src/codegen"
                "${PROJECT_BINARY_DIR}/src/symbols"
                "${PROJECT_BINARY_DIR}/src/parser")

add_subdirectory(parser)
add_subdirectory(semantics)
add_subdirectory(codegen)
add_subdirectory(mjc)
