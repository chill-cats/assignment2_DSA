cmake_minimum_required(VERSION 3.15)

project(myassignment CXX)

set(ignoreMe "${CMAKE_C_COMPILER}")

include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

# Link with this 'library' to set the c++ standard / compile-time options requested
add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE cxx_std_14)

if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang")
    option(ENABLE_BUILD_WITH_TIME_TRACE "Enable -ftime-trace to generate time tracing .json files on clang" OFF)
    if(ENABLE_BUILD_WITH_TIME_TRACE)
        target_compile_options(project_options INTERFACE -ftime-trace)
    endif()
endif()

# Link with this 'library' to use the warning specified in CompilerWarnings.cmake
add_library(project_warnings INTERFACE)

# Enable cache system
include(cmake/Cache.cmake)

# Add linker configuration
include(cmake/Linker.cmake)
configure_linker(project_options)

# standard compiler warnings
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

# sanitizer option if supported by compiler
include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

# allow for static analysis option
include(cmake/StaticAnalyzers.cmake)

option(ENABLE_TESTING "Enable Test Builds" ON)
if (ENABLE_TESTING)
    enable_testing()
    add_subdirectory(test)
    add_compile_definitions(TESTING)
endif()

option(ENABLE_PCH "Enalbe precompiled Headers" OFF)
if (ENABLE_PCH)
    target_precompile_headers(
        project_options
        INTERFACE
        <string>
        <iostream>
        <fstream>
        <regex>)
endif()

option(ENABLE_CONAN "Use conan for dependency management" OFF)
if (ENABLE_CONAN)
    include(cmake/Conan.cmake)
    run_conan()
endif()

option(ENABLE_COC_CLANGD_CONFIG "This generate nvim coc config file from template in the .vim folder" OFF)
if (ENABLE_COC_CLANGD_CONFIG)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/.vim/coc-settings.json.in
                ${CMAKE_CURRENT_SOURCE_DIR}/.vim/coc-settings.json)
endif()

option(USE_EXPERIMENTAL_PARSING "This turn on experimental parsing code instead of regex" OFF)
if (USE_EXPERIMENTAL_PARSING)
    add_definitions(-DEXPERIMENTAL_PARSING)
endif()

add_subdirectory(src)
# add_subdirectory(src-refactor)
