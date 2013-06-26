
#add_executable(test test.cpp)
#target_link_libraries(test ${LLVM_LIB})

set(BIN ${CMAKE_CURRENT_BINARY_DIR})
set(SRC ${CMAKE_CURRENT_SOURCE_DIR})

include_directories("${PROJECT_BINARY_DIR}/src/parser")
include_directories ("${PROJECT_SOURCE_DIR}/src")

set(CP ${CMAKE_COMMAND} -E copy)

add_executable(test-parser parser.cpp)
target_link_libraries(test-parser ${ANTLR_LIB} Parser)

file(GLOB programs "*.mj")
set(plist)
foreach(p ${programs})
    get_filename_component(pname ${p} NAME)
    list(APPEND plist ${BIN}/${pname})
    add_custom_command(COMMAND ${CMAKE_COMMAND} 
        ARGS -E copy ${SRC}/${pname} ${BIN}
        OUTPUT ${BIN}/${pname}
        DEPENDS ${SRC}/${pname})
endforeach()

add_custom_target(CopyTestPrograms ALL
    DEPENDS ${plist})

add_executable(test-symbol symbols.cpp)
target_link_libraries(test-symbol MjSemantics ${LLVM_LIB})

add_executable(test-semantics semantics.cpp)
target_link_libraries(test-semantics MjSemantics Parser ${LLVM_LIB} ${ANTLR_LIB})

enable_testing()

add_test(parser ${BIN}/test-parser ${BIN}/test.mj)
add_test(symbols ${BIN}/test-symbol)
add_test(semantics ${BIN}/test-semantics)
#add_test(parsertest ) 

