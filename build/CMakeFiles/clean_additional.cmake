# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "")
  file(REMOVE_RECURSE
  "CMakeFiles\\MiniOSKernelSimulator_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MiniOSKernelSimulator_autogen.dir\\ParseCache.txt"
  "MiniOSKernelSimulator_autogen"
  "_deps\\catch2-build\\src\\CMakeFiles\\Catch2WithMain_autogen.dir\\AutogenUsed.txt"
  "_deps\\catch2-build\\src\\CMakeFiles\\Catch2WithMain_autogen.dir\\ParseCache.txt"
  "_deps\\catch2-build\\src\\CMakeFiles\\Catch2_autogen.dir\\AutogenUsed.txt"
  "_deps\\catch2-build\\src\\CMakeFiles\\Catch2_autogen.dir\\ParseCache.txt"
  "_deps\\catch2-build\\src\\Catch2WithMain_autogen"
  "_deps\\catch2-build\\src\\Catch2_autogen"
  "_deps\\rapidcheck-build\\CMakeFiles\\rapidcheck_autogen.dir\\AutogenUsed.txt"
  "_deps\\rapidcheck-build\\CMakeFiles\\rapidcheck_autogen.dir\\ParseCache.txt"
  "_deps\\rapidcheck-build\\rapidcheck_autogen"
  "tests\\CMakeFiles\\MiniOSIntegrationTests_autogen.dir\\AutogenUsed.txt"
  "tests\\CMakeFiles\\MiniOSIntegrationTests_autogen.dir\\ParseCache.txt"
  "tests\\CMakeFiles\\MiniOSPropertyTests_autogen.dir\\AutogenUsed.txt"
  "tests\\CMakeFiles\\MiniOSPropertyTests_autogen.dir\\ParseCache.txt"
  "tests\\MiniOSIntegrationTests_autogen"
  "tests\\MiniOSPropertyTests_autogen"
  )
endif()
