cmake_minimum_required(VERSION 3.10.0)

project(json LANGUAGES C VERSION 0.1.0)

set(CMAKE_C_STANDARD 99)

option(DEBUG_MODE OFF)
option(COMPILE_TESTS ON) # for some reason the defualt doesn't work...gotta love CMake...

if (DEBUG_MODE)
  message("Compiling in debug mode...")
  set(CMAKE_C_FLAGS "-O0 -g -Wall -Wextra -Wpedantic")
else()
  message("Compiling in release mode...")
  set(CMAKE_C_FLAGS "-O2 -Wall -Wextra -Wpedantic")
endif()

add_subdirectory(src)

if (COMPILE_TESTS)
  message("Compiling tests...")
  include(CTest)
  enable_testing()

  add_subdirectory(test)

  file(COPY test/sample_file.json DESTINATION test)
  file(COPY test/complex_file.json DESTINATION test)
endif()

unset(DEBUG_MODE)
unset(COMPILE_TESTS)
