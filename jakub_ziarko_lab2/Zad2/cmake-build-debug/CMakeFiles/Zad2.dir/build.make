# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.9

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /home/jakub/IntelliJ/apps/CLion/ch-0/173.4674.29/bin/cmake/bin/cmake

# The command to remove a file.
RM = /home/jakub/IntelliJ/apps/CLion/ch-0/173.4674.29/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/jakub/CLionProjects/Lab2/Zad2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/jakub/CLionProjects/Lab2/Zad2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/Zad2.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/Zad2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Zad2.dir/flags.make

CMakeFiles/Zad2.dir/main.cpp.o: CMakeFiles/Zad2.dir/flags.make
CMakeFiles/Zad2.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/jakub/CLionProjects/Lab2/Zad2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Zad2.dir/main.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/Zad2.dir/main.cpp.o -c /home/jakub/CLionProjects/Lab2/Zad2/main.cpp

CMakeFiles/Zad2.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/Zad2.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/jakub/CLionProjects/Lab2/Zad2/main.cpp > CMakeFiles/Zad2.dir/main.cpp.i

CMakeFiles/Zad2.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/Zad2.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/jakub/CLionProjects/Lab2/Zad2/main.cpp -o CMakeFiles/Zad2.dir/main.cpp.s

CMakeFiles/Zad2.dir/main.cpp.o.requires:

.PHONY : CMakeFiles/Zad2.dir/main.cpp.o.requires

CMakeFiles/Zad2.dir/main.cpp.o.provides: CMakeFiles/Zad2.dir/main.cpp.o.requires
	$(MAKE) -f CMakeFiles/Zad2.dir/build.make CMakeFiles/Zad2.dir/main.cpp.o.provides.build
.PHONY : CMakeFiles/Zad2.dir/main.cpp.o.provides

CMakeFiles/Zad2.dir/main.cpp.o.provides.build: CMakeFiles/Zad2.dir/main.cpp.o


# Object files for target Zad2
Zad2_OBJECTS = \
"CMakeFiles/Zad2.dir/main.cpp.o"

# External object files for target Zad2
Zad2_EXTERNAL_OBJECTS =

Zad2: CMakeFiles/Zad2.dir/main.cpp.o
Zad2: CMakeFiles/Zad2.dir/build.make
Zad2: CMakeFiles/Zad2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/jakub/CLionProjects/Lab2/Zad2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable Zad2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Zad2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Zad2.dir/build: Zad2

.PHONY : CMakeFiles/Zad2.dir/build

CMakeFiles/Zad2.dir/requires: CMakeFiles/Zad2.dir/main.cpp.o.requires

.PHONY : CMakeFiles/Zad2.dir/requires

CMakeFiles/Zad2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Zad2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Zad2.dir/clean

CMakeFiles/Zad2.dir/depend:
	cd /home/jakub/CLionProjects/Lab2/Zad2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/jakub/CLionProjects/Lab2/Zad2 /home/jakub/CLionProjects/Lab2/Zad2 /home/jakub/CLionProjects/Lab2/Zad2/cmake-build-debug /home/jakub/CLionProjects/Lab2/Zad2/cmake-build-debug /home/jakub/CLionProjects/Lab2/Zad2/cmake-build-debug/CMakeFiles/Zad2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/Zad2.dir/depend

