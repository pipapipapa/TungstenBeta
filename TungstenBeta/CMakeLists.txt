cmake_minimum_required(VERSION 3.23)

set(project "TungstenBeta")
project(${project})

set(CMAKE_CXX_STANDARD 17)

set(${project}_SOURCES
        TungstenBeta.cpp
        operators.cpp
        ElementaryFunctions.cpp
        Constant.cpp
        Variable.cpp
        Methods.cpp)

set(${project}_HEADERS
        TungstenBeta.h 
        operators.h 
        ElementaryFunctions.h 
        Constant.h 
        Variable.h 
        Methods.h)

set(${project}_SOURCE_LIST
        ${${project}_SOURCES}
        ${${project}_HEADERS})

add_library(${project}
        STATIC
        ${${project}_SOURCE_LIST})
