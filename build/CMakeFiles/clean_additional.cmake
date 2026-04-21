# Additional clean files
cmake_minimum_required(VERSION 3.16)

if("${CONFIG}" STREQUAL "" OR "${CONFIG}" STREQUAL "Debug")
  file(REMOVE_RECURSE
  "CMakeFiles\\MiniOSKernelSimulator_autogen.dir\\AutogenUsed.txt"
  "CMakeFiles\\MiniOSKernelSimulator_autogen.dir\\ParseCache.txt"
  "MiniOSKernelSimulator_autogen"
  "tests\\CMakeFiles\\MiniOSIntegrationTests_autogen.dir\\AutogenUsed.txt"
  "tests\\CMakeFiles\\MiniOSIntegrationTests_autogen.dir\\ParseCache.txt"
  "tests\\MiniOSIntegrationTests_autogen"
  )
endif()
