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

# Include any dependencies generated for this target.
include libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/depend.make

# Include the progress variables for this target.
include libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/progress.make

# Include the compile flags for this target's objects.
include libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/flags.make

libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.o: libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/flags.make
libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.o: ../libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/k/CLionProjects/libsnark/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.o"
	cd /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.o -c /home/k/CLionProjects/libsnark/libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp

libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.i"
	cd /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/k/CLionProjects/libsnark/libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp > CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.i

libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.s"
	cd /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/k/CLionProjects/libsnark/libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp -o CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.s

# Object files for target zk_proof_systems_r1cs_ppzksnark_test
zk_proof_systems_r1cs_ppzksnark_test_OBJECTS = \
"CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.o"

# External object files for target zk_proof_systems_r1cs_ppzksnark_test
zk_proof_systems_r1cs_ppzksnark_test_EXTERNAL_OBJECTS =

libsnark/zk_proof_systems_r1cs_ppzksnark_test: libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/zk_proof_systems/ppzksnark/r1cs_ppzksnark/tests/test_r1cs_ppzksnark.cpp.o
libsnark/zk_proof_systems_r1cs_ppzksnark_test: libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/build.make
libsnark/zk_proof_systems_r1cs_ppzksnark_test: libsnark/libsnarkd.a
libsnark/zk_proof_systems_r1cs_ppzksnark_test: depends/libff/libff/libffd.a
libsnark/zk_proof_systems_r1cs_ppzksnark_test: /usr/lib/x86_64-linux-gnu/libgmp.so
libsnark/zk_proof_systems_r1cs_ppzksnark_test: /usr/lib/x86_64-linux-gnu/libgmp.so
libsnark/zk_proof_systems_r1cs_ppzksnark_test: /usr/lib/x86_64-linux-gnu/libgmpxx.so
libsnark/zk_proof_systems_r1cs_ppzksnark_test: depends/libzmd.a
libsnark/zk_proof_systems_r1cs_ppzksnark_test: libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/k/CLionProjects/libsnark/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable zk_proof_systems_r1cs_ppzksnark_test"
	cd /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/build: libsnark/zk_proof_systems_r1cs_ppzksnark_test

.PHONY : libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/build

libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/clean:
	cd /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark && $(CMAKE_COMMAND) -P CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/cmake_clean.cmake
.PHONY : libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/clean

libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/depend:
	cd /home/k/CLionProjects/libsnark/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/k/CLionProjects/libsnark /home/k/CLionProjects/libsnark/libsnark /home/k/CLionProjects/libsnark/cmake-build-debug /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark /home/k/CLionProjects/libsnark/cmake-build-debug/libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : libsnark/CMakeFiles/zk_proof_systems_r1cs_ppzksnark_test.dir/depend
