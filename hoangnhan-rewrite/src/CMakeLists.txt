cmake_minimum_required(VERSION 3.15)

add_executable(main main.cpp)
target_link_libraries(
    main
    PRIVATE project_options
            project_warnings)
add_library(symbolTable SymbolTable.cpp SymbolTable.h error.h main.h)
target_include_directories(symbolTable INTERFACE .)

target_link_libraries(
    symbolTable
    PRIVATE project_options
            project_warnings)
target_link_libraries(main PRIVATE symbolTable)
