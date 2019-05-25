cmake_minimum_required(VERSION 3.10)
project(Hw3)

set(CMAKE_CXX_STANDARD 98)
FIND_PACKAGE(BISON)
FIND_PACKAGE(FLEX)

BISON_TARGET(
        Hw3Bison
        parser.ypp ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.cpp
        COMPILE_FLAGS -v

)

FLEX_TARGET(
        Hw3Lexer
        scanner.lex ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c
)

ADD_FLEX_BISON_DEPENDENCY(Hw3Lexer Hw3Bison)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_BINARY_DIR})

ADD_EXECUTABLE(Hw3
        output.cpp
        parser.ypp
        scanner.lex
        ${BISON_Hw3Bison_OUTPUTS}
        ${FLEX_Hw3Lexer_OUTPUTS}
        )