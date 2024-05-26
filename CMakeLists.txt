cmake_minimum_required(VERSION 3.23)

project(TungstenBetaDebug)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Sources for the main executable
set(TungstenBetaDebug_SOURCES
        main.cpp)

# Add TungstenBeta as a subdirectory (assuming it's a library)
add_subdirectory(TungstenBeta)

# Find GTK+ and PkgConfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK4 REQUIRED gtk4)

# Set include and library directories for GTK+
include_directories(${GTK4_INCLUDE_DIRS})
link_directories(${GTK4_LIB_DIRS}) 


# Build the executable
add_executable(TungstenBetaDebug 
        ${TungstenBetaDebug_SOURCES})

# Link the libraries
target_link_libraries(TungstenBetaDebug
        TungstenBeta 
        ${GTK4_LIBRARIES})