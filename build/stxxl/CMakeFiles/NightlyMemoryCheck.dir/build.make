# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

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
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/arghya/cache

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/arghya/cache/build

# Utility rule file for NightlyMemoryCheck.

# Include the progress variables for this target.
include stxxl/CMakeFiles/NightlyMemoryCheck.dir/progress.make

stxxl/CMakeFiles/NightlyMemoryCheck:
	cd /home/arghya/cache/build/stxxl && /usr/bin/ctest -D NightlyMemoryCheck

NightlyMemoryCheck: stxxl/CMakeFiles/NightlyMemoryCheck
NightlyMemoryCheck: stxxl/CMakeFiles/NightlyMemoryCheck.dir/build.make

.PHONY : NightlyMemoryCheck

# Rule to build all files generated by this target.
stxxl/CMakeFiles/NightlyMemoryCheck.dir/build: NightlyMemoryCheck

.PHONY : stxxl/CMakeFiles/NightlyMemoryCheck.dir/build

stxxl/CMakeFiles/NightlyMemoryCheck.dir/clean:
	cd /home/arghya/cache/build/stxxl && $(CMAKE_COMMAND) -P CMakeFiles/NightlyMemoryCheck.dir/cmake_clean.cmake
.PHONY : stxxl/CMakeFiles/NightlyMemoryCheck.dir/clean

stxxl/CMakeFiles/NightlyMemoryCheck.dir/depend:
	cd /home/arghya/cache/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/arghya/cache /home/arghya/cache/stxxl /home/arghya/cache/build /home/arghya/cache/build/stxxl /home/arghya/cache/build/stxxl/CMakeFiles/NightlyMemoryCheck.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : stxxl/CMakeFiles/NightlyMemoryCheck.dir/depend

