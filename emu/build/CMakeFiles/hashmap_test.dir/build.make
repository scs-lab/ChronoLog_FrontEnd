# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.25

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
CMAKE_COMMAND = /home/aparna/spack/opt/spack/linux-ubuntu22.04-zen3/gcc-11.3.0/cmake-3.25.0-pdcwmffx3mly7kjg3gubnh62zpxyz7du/bin/cmake

# The command to remove a file.
RM = /home/aparna/spack/opt/spack/linux-ubuntu22.04-zen3/gcc-11.3.0/cmake-3.25.0-pdcwmffx3mly7kjg3gubnh62zpxyz7du/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/aparna/Desktop/frontend/Emulator/emu

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aparna/Desktop/frontend/Emulator/emu/build

# Include any dependencies generated for this target.
include CMakeFiles/hashmap_test.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/hashmap_test.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/hashmap_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/hashmap_test.dir/flags.make

CMakeFiles/hashmap_test.dir/main.o: CMakeFiles/hashmap_test.dir/flags.make
CMakeFiles/hashmap_test.dir/main.o: /home/aparna/Desktop/frontend/Emulator/emu/main.cpp
CMakeFiles/hashmap_test.dir/main.o: CMakeFiles/hashmap_test.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/aparna/Desktop/frontend/Emulator/emu/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/hashmap_test.dir/main.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/hashmap_test.dir/main.o -MF CMakeFiles/hashmap_test.dir/main.o.d -o CMakeFiles/hashmap_test.dir/main.o -c /home/aparna/Desktop/frontend/Emulator/emu/main.cpp

CMakeFiles/hashmap_test.dir/main.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/hashmap_test.dir/main.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/aparna/Desktop/frontend/Emulator/emu/main.cpp > CMakeFiles/hashmap_test.dir/main.i

CMakeFiles/hashmap_test.dir/main.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/hashmap_test.dir/main.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/aparna/Desktop/frontend/Emulator/emu/main.cpp -o CMakeFiles/hashmap_test.dir/main.s

# Object files for target hashmap_test
hashmap_test_OBJECTS = \
"CMakeFiles/hashmap_test.dir/main.o"

# External object files for target hashmap_test
hashmap_test_EXTERNAL_OBJECTS =

hashmap_test: CMakeFiles/hashmap_test.dir/main.o
hashmap_test: CMakeFiles/hashmap_test.dir/build.make
hashmap_test: CMakeFiles/hashmap_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/aparna/Desktop/frontend/Emulator/emu/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable hashmap_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/hashmap_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/hashmap_test.dir/build: hashmap_test
.PHONY : CMakeFiles/hashmap_test.dir/build

CMakeFiles/hashmap_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/hashmap_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/hashmap_test.dir/clean

CMakeFiles/hashmap_test.dir/depend:
	cd /home/aparna/Desktop/frontend/Emulator/emu/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aparna/Desktop/frontend/Emulator/emu /home/aparna/Desktop/frontend/Emulator/emu /home/aparna/Desktop/frontend/Emulator/emu/build /home/aparna/Desktop/frontend/Emulator/emu/build /home/aparna/Desktop/frontend/Emulator/emu/build/CMakeFiles/hashmap_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/hashmap_test.dir/depend

