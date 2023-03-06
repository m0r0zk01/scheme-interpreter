cmake_minimum_required(VERSION 3.20)

project(SchemeInterpreter)

set(CMAKE_CXX_STANDARD 20)

include_directories(inc)

add_library(tokenizer src/tokenizer.cpp)
add_library(parser src/parser.cpp)
add_library(object src/object.cpp)
add_library(scheme src/scheme.cpp)

link_libraries(
    scheme
    object
    parser
    tokenizer
)

add_executable(repl main.cpp)

# tests

enable_testing()
add_subdirectory(contrib)
add_subdirectory(tests)
