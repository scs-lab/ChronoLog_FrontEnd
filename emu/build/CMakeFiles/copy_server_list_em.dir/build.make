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

# Utility rule file for copy_server_list_em.

# Include any custom commands dependencies for this target.
include CMakeFiles/copy_server_list_em.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/copy_server_list_em.dir/progress.make

copy_server_list_em: CMakeFiles/copy_server_list_em.dir/build.make
	/home/aparna/spack/opt/spack/linux-ubuntu22.04-zen3/gcc-11.3.0/cmake-3.25.0-pdcwmffx3mly7kjg3gubnh62zpxyz7du/bin/cmake -E copy /home/aparna/Desktop/frontend/Emulator/emu/server_list /home/aparna/Desktop/frontend/Emulator/emu/build
.PHONY : copy_server_list_em

# Rule to build all files generated by this target.
CMakeFiles/copy_server_list_em.dir/build: copy_server_list_em
.PHONY : CMakeFiles/copy_server_list_em.dir/build

CMakeFiles/copy_server_list_em.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/copy_server_list_em.dir/cmake_clean.cmake
.PHONY : CMakeFiles/copy_server_list_em.dir/clean

CMakeFiles/copy_server_list_em.dir/depend:
	cd /home/aparna/Desktop/frontend/Emulator/emu/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aparna/Desktop/frontend/Emulator/emu /home/aparna/Desktop/frontend/Emulator/emu /home/aparna/Desktop/frontend/Emulator/emu/build /home/aparna/Desktop/frontend/Emulator/emu/build /home/aparna/Desktop/frontend/Emulator/emu/build/CMakeFiles/copy_server_list_em.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/copy_server_list_em.dir/depend

