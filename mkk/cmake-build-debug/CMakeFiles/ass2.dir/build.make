# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.20

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

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /var/lib/snapd/snap/clion/164/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /var/lib/snapd/snap/clion/164/bin/cmake/linux/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/ngmkhanh/CLionProjects/ass2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/ngmkhanh/CLionProjects/ass2/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/ass2.dir/depend.make
# Include the progress variables for this target.
include CMakeFiles/ass2.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/ass2.dir/flags.make

CMakeFiles/ass2.dir/main.cpp.o: CMakeFiles/ass2.dir/flags.make
CMakeFiles/ass2.dir/main.cpp.o: ../main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/ngmkhanh/CLionProjects/ass2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/ass2.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/ass2.dir/main.cpp.o -c /home/ngmkhanh/CLionProjects/ass2/main.cpp

CMakeFiles/ass2.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/ass2.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/ngmkhanh/CLionProjects/ass2/main.cpp > CMakeFiles/ass2.dir/main.cpp.i

CMakeFiles/ass2.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/ass2.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/ngmkhanh/CLionProjects/ass2/main.cpp -o CMakeFiles/ass2.dir/main.cpp.s

# Object files for target ass2
ass2_OBJECTS = \
"CMakeFiles/ass2.dir/main.cpp.o"

# External object files for target ass2
ass2_EXTERNAL_OBJECTS =

ass2: CMakeFiles/ass2.dir/main.cpp.o
ass2: CMakeFiles/ass2.dir/build.make
ass2: CMakeFiles/ass2.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/ngmkhanh/CLionProjects/ass2/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable ass2"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/ass2.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/ass2.dir/build: ass2
.PHONY : CMakeFiles/ass2.dir/build

CMakeFiles/ass2.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/ass2.dir/cmake_clean.cmake
.PHONY : CMakeFiles/ass2.dir/clean

CMakeFiles/ass2.dir/depend:
	cd /home/ngmkhanh/CLionProjects/ass2/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/ngmkhanh/CLionProjects/ass2 /home/ngmkhanh/CLionProjects/ass2 /home/ngmkhanh/CLionProjects/ass2/cmake-build-debug /home/ngmkhanh/CLionProjects/ass2/cmake-build-debug /home/ngmkhanh/CLionProjects/ass2/cmake-build-debug/CMakeFiles/ass2.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/ass2.dir/depend

