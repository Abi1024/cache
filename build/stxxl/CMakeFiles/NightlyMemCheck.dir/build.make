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
CMAKE_SOURCE_DIR = /home/abiyaz/cache

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/abiyaz/cache/build

# Utility rule file for NightlyMemCheck.

# Include the progress variables for this target.
include stxxl/CMakeFiles/NightlyMemCheck.dir/progress.make

stxxl/CMakeFiles/NightlyMemCheck:
	cd /home/abiyaz/cache/build/stxxl && /usr/bin/ctest -D NightlyMemCheck

NightlyMemCheck: stxxl/CMakeFiles/NightlyMemCheck
NightlyMemCheck: stxxl/CMakeFiles/NightlyMemCheck.dir/build.make

.PHONY : NightlyMemCheck

# Rule to build all files generated by this target.
stxxl/CMakeFiles/NightlyMemCheck.dir/build: NightlyMemCheck

.PHONY : stxxl/CMakeFiles/NightlyMemCheck.dir/build

stxxl/CMakeFiles/NightlyMemCheck.dir/clean:
	cd /home/abiyaz/cache/build/stxxl && $(CMAKE_COMMAND) -P CMakeFiles/NightlyMemCheck.dir/cmake_clean.cmake
.PHONY : stxxl/CMakeFiles/NightlyMemCheck.dir/clean

stxxl/CMakeFiles/NightlyMemCheck.dir/depend:
	cd /home/abiyaz/cache/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/abiyaz/cache /home/abiyaz/cache/stxxl /home/abiyaz/cache/build /home/abiyaz/cache/build/stxxl /home/abiyaz/cache/build/stxxl/CMakeFiles/NightlyMemCheck.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : stxxl/CMakeFiles/NightlyMemCheck.dir/depend

