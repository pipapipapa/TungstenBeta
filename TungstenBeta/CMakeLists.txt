cmake_minimum_required(VERSION 3.23)
project(TungstenBeta)

set(CMAKE_CXX_STANDARD 17)

set(TungstenBeta_SOURCES
    TungstenBeta.cpp
    TungstenBetaGUI.cpp
    operators.cpp
    ElementaryFunctions.cpp
    Constant.cpp
    Variable.cpp
    Methods.cpp)

set(TungstenBeta_HEADERS
    TungstenBeta.h
    TungstenBetaGUI.h
    operators.h
    ElementaryFunctions.h
    Constant.h
    Variable.h
    Methods.h)

add_library(TungstenBeta STATIC
    ${TungstenBeta_SOURCES}
    ${TungstenBeta_HEADERS})


find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIB_DIRS}) 


target_link_libraries(
    TungstenBeta
    ${GTK4_LIBRARIES})