# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.22.1/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.22.1/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug

# Include any dependencies generated for this target.
include unittest/lib/googletest/CMakeFiles/gtest.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include unittest/lib/googletest/CMakeFiles/gtest.dir/compiler_depend.make

# Include the progress variables for this target.
include unittest/lib/googletest/CMakeFiles/gtest.dir/progress.make

# Include the compile flags for this target's objects.
include unittest/lib/googletest/CMakeFiles/gtest.dir/flags.make

unittest/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: unittest/lib/googletest/CMakeFiles/gtest.dir/flags.make
unittest/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: ../unittest/lib/googletest/src/gtest-all.cc
unittest/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o: unittest/lib/googletest/CMakeFiles/gtest.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object unittest/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o"
	cd /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/unittest/lib/googletest && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT unittest/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o -MF CMakeFiles/gtest.dir/src/gtest-all.cc.o.d -o CMakeFiles/gtest.dir/src/gtest-all.cc.o -c /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/unittest/lib/googletest/src/gtest-all.cc

unittest/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/gtest.dir/src/gtest-all.cc.i"
	cd /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/unittest/lib/googletest && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/unittest/lib/googletest/src/gtest-all.cc > CMakeFiles/gtest.dir/src/gtest-all.cc.i

unittest/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/gtest.dir/src/gtest-all.cc.s"
	cd /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/unittest/lib/googletest && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/unittest/lib/googletest/src/gtest-all.cc -o CMakeFiles/gtest.dir/src/gtest-all.cc.s

# Object files for target gtest
gtest_OBJECTS = \
"CMakeFiles/gtest.dir/src/gtest-all.cc.o"

# External object files for target gtest
gtest_EXTERNAL_OBJECTS =

lib/libgtestd.a: unittest/lib/googletest/CMakeFiles/gtest.dir/src/gtest-all.cc.o
lib/libgtestd.a: unittest/lib/googletest/CMakeFiles/gtest.dir/build.make
lib/libgtestd.a: unittest/lib/googletest/CMakeFiles/gtest.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX static library ../../../lib/libgtestd.a"
	cd /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/unittest/lib/googletest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean_target.cmake
	cd /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/unittest/lib/googletest && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/gtest.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
unittest/lib/googletest/CMakeFiles/gtest.dir/build: lib/libgtestd.a
.PHONY : unittest/lib/googletest/CMakeFiles/gtest.dir/build

unittest/lib/googletest/CMakeFiles/gtest.dir/clean:
	cd /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/unittest/lib/googletest && $(CMAKE_COMMAND) -P CMakeFiles/gtest.dir/cmake_clean.cmake
.PHONY : unittest/lib/googletest/CMakeFiles/gtest.dir/clean

unittest/lib/googletest/CMakeFiles/gtest.dir/depend:
	cd /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/unittest/lib/googletest /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/unittest/lib/googletest /Users/jjangchan/workspace/Clion/chan-project/crypto-exchange-api/cmake-build-debug/unittest/lib/googletest/CMakeFiles/gtest.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : unittest/lib/googletest/CMakeFiles/gtest.dir/depend

