# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /home/k/Desktop/clion-2018.2.2/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /home/k/Desktop/clion-2018.2.2/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/k/CLionProjects/libsnark

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/k/CLionProjects/libsnark/cmake-build-debug

# Utility rule file for NightlyMemCheck.

# Include the progress variables for this target.
include libsnark/CMakeFiles/NightlyMemCheck.dir/progress.make

libsnark/CMakeFiles/NightlyMemCheck:
	cd /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark && /home/k/Desktop/clion-2018.2.2/bin/cmake/linux/bin/ctest -D NightlyMemCheck

NightlyMemCheck: libsnark/CMakeFiles/NightlyMemCheck
NightlyMemCheck: libsnark/CMakeFiles/NightlyMemCheck.dir/build.make

.PHONY : NightlyMemCheck

# Rule to build all files generated by this target.
libsnark/CMakeFiles/NightlyMemCheck.dir/build: NightlyMemCheck

.PHONY : libsnark/CMakeFiles/NightlyMemCheck.dir/build

libsnark/CMakeFiles/NightlyMemCheck.dir/clean:
	cd /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark && $(CMAKE_COMMAND) -P CMakeFiles/NightlyMemCheck.dir/cmake_clean.cmake
.PHONY : libsnark/CMakeFiles/NightlyMemCheck.dir/clean

libsnark/CMakeFiles/NightlyMemCheck.dir/depend:
	cd /home/k/CLionProjects/libsnark/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/k/CLionProjects/libsnark /home/k/CLionProjects/libsnark/libsnark /home/k/CLionProjects/libsnark/cmake-build-debug /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark/CMakeFiles/NightlyMemCheck.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libsnark/CMakeFiles/NightlyMemCheck.dir/depend
