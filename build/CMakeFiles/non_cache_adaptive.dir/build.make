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

# Include any dependencies generated for this target.
include CMakeFiles/non_cache_adaptive.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/non_cache_adaptive.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/non_cache_adaptive.dir/flags.make

CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o: CMakeFiles/non_cache_adaptive.dir/flags.make
CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o: ../non_cache_adaptive.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/abiyaz/cache/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o -c /home/abiyaz/cache/non_cache_adaptive.cpp

CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/abiyaz/cache/non_cache_adaptive.cpp > CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.i

CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/abiyaz/cache/non_cache_adaptive.cpp -o CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.s

CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o.requires:

.PHONY : CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o.requires

CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o.provides: CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o.requires
	$(MAKE) -f CMakeFiles/non_cache_adaptive.dir/build.make CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o.provides.build
.PHONY : CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o.provides

CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o.provides.build: CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o


# Object files for target non_cache_adaptive
non_cache_adaptive_OBJECTS = \
"CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o"

# External object files for target non_cache_adaptive
non_cache_adaptive_EXTERNAL_OBJECTS =

non_cache_adaptive: CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o
non_cache_adaptive: CMakeFiles/non_cache_adaptive.dir/build.make
non_cache_adaptive: CMakeFiles/non_cache_adaptive.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/abiyaz/cache/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable non_cache_adaptive"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/non_cache_adaptive.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/non_cache_adaptive.dir/build: non_cache_adaptive

.PHONY : CMakeFiles/non_cache_adaptive.dir/build

CMakeFiles/non_cache_adaptive.dir/requires: CMakeFiles/non_cache_adaptive.dir/non_cache_adaptive.cpp.o.requires

.PHONY : CMakeFiles/non_cache_adaptive.dir/requires

CMakeFiles/non_cache_adaptive.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/non_cache_adaptive.dir/cmake_clean.cmake
.PHONY : CMakeFiles/non_cache_adaptive.dir/clean

CMakeFiles/non_cache_adaptive.dir/depend:
	cd /home/abiyaz/cache/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/abiyaz/cache /home/abiyaz/cache /home/abiyaz/cache/build /home/abiyaz/cache/build /home/abiyaz/cache/build/CMakeFiles/non_cache_adaptive.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/non_cache_adaptive.dir/depend

