# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.27

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\User\Projects\TungstenBeta

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\User\Projects\TungstenBeta\build

# Include any dependencies generated for this target.
include TungstenBeta/CMakeFiles/TungstenBeta.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include TungstenBeta/CMakeFiles/TungstenBeta.dir/compiler_depend.make

# Include the progress variables for this target.
include TungstenBeta/CMakeFiles/TungstenBeta.dir/progress.make

# Include the compile flags for this target's objects.
include TungstenBeta/CMakeFiles/TungstenBeta.dir/flags.make

TungstenBeta/CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.obj: TungstenBeta/CMakeFiles/TungstenBeta.dir/flags.make
TungstenBeta/CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.obj: C:/Users/User/Projects/TungstenBeta/TungstenBeta/TungstenBeta.cpp
TungstenBeta/CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.obj: TungstenBeta/CMakeFiles/TungstenBeta.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=C:\Users\User\Projects\TungstenBeta\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object TungstenBeta/CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.obj"
	cd /d C:\Users\User\Projects\TungstenBeta\build\TungstenBeta && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT TungstenBeta/CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.obj -MF CMakeFiles\TungstenBeta.dir\TungstenBeta.cpp.obj.d -o CMakeFiles\TungstenBeta.dir\TungstenBeta.cpp.obj -c C:\Users\User\Projects\TungstenBeta\TungstenBeta\TungstenBeta.cpp

TungstenBeta/CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.i"
	cd /d C:\Users\User\Projects\TungstenBeta\build\TungstenBeta && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\User\Projects\TungstenBeta\TungstenBeta\TungstenBeta.cpp > CMakeFiles\TungstenBeta.dir\TungstenBeta.cpp.i

TungstenBeta/CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.s"
	cd /d C:\Users\User\Projects\TungstenBeta\build\TungstenBeta && C:\msys64\mingw64\bin\g++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S C:\Users\User\Projects\TungstenBeta\TungstenBeta\TungstenBeta.cpp -o CMakeFiles\TungstenBeta.dir\TungstenBeta.cpp.s

# Object files for target TungstenBeta
TungstenBeta_OBJECTS = \
"CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.obj"

# External object files for target TungstenBeta
TungstenBeta_EXTERNAL_OBJECTS =

TungstenBeta/libTungstenBeta.a: TungstenBeta/CMakeFiles/TungstenBeta.dir/TungstenBeta.cpp.obj
TungstenBeta/libTungstenBeta.a: TungstenBeta/CMakeFiles/TungstenBeta.dir/build.make
TungstenBeta/libTungstenBeta.a: TungstenBeta/CMakeFiles/TungstenBeta.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=C:\Users\User\Projects\TungstenBeta\build\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library libTungstenBeta.a"
	cd /d C:\Users\User\Projects\TungstenBeta\build\TungstenBeta && $(CMAKE_COMMAND) -P CMakeFiles\TungstenBeta.dir\cmake_clean_target.cmake
	cd /d C:\Users\User\Projects\TungstenBeta\build\TungstenBeta && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\TungstenBeta.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
TungstenBeta/CMakeFiles/TungstenBeta.dir/build: TungstenBeta/libTungstenBeta.a
.PHONY : TungstenBeta/CMakeFiles/TungstenBeta.dir/build

TungstenBeta/CMakeFiles/TungstenBeta.dir/clean:
	cd /d C:\Users\User\Projects\TungstenBeta\build\TungstenBeta && $(CMAKE_COMMAND) -P CMakeFiles\TungstenBeta.dir\cmake_clean.cmake
.PHONY : TungstenBeta/CMakeFiles/TungstenBeta.dir/clean

TungstenBeta/CMakeFiles/TungstenBeta.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\Users\User\Projects\TungstenBeta C:\Users\User\Projects\TungstenBeta\TungstenBeta C:\Users\User\Projects\TungstenBeta\build C:\Users\User\Projects\TungstenBeta\build\TungstenBeta C:\Users\User\Projects\TungstenBeta\build\TungstenBeta\CMakeFiles\TungstenBeta.dir\DependInfo.cmake "--color=$(COLOR)"
.PHONY : TungstenBeta/CMakeFiles/TungstenBeta.dir/depend

