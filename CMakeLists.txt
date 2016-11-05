cmake_minimum_required(VERSION 3.6)
project(e2ml)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(SOURCE_FILES main.cpp
        include/e2ml.h
        include/exceptions.h
        include/values.h
        src/e2ml.cpp
        src/parser.cpp
        src/lexer.cpp)
add_executable(e2ml ${SOURCE_FILES})
