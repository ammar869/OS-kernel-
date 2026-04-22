# CMake generated Testfile for 
# Source directory: C:/Users/Ammar/Documents/Kernal/tests
# Build directory: C:/Users/Ammar/Documents/Kernal/build/tests
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
include("C:/Users/Ammar/Documents/Kernal/build/tests/MiniOSPropertyTests-b12d07c_include.cmake")
add_test(IntegrationTests "C:/Users/Ammar/Documents/Kernal/build/bin/MiniOSIntegrationTests.exe")
set_tests_properties(IntegrationTests PROPERTIES  _BACKTRACE_TRIPLES "C:/Users/Ammar/Documents/Kernal/tests/CMakeLists.txt;42;add_test;C:/Users/Ammar/Documents/Kernal/tests/CMakeLists.txt;0;")
subdirs("../_deps/catch2-build")
subdirs("../_deps/rapidcheck-build")
