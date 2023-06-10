cmake_minimum_required(VERSION 3.10.0)

project(json C)

set(CMAKE_C_STANDARD 99)

#debug
set(CMAKE_C_FLAGS "-O0 -g -Wall -Wextra -Wpedantic")

#release
#set(CMAKE_C_FLAGS "-O2 -Wall -Wextra -Wpedantic")

add_subdirectory(src)

include(CTest)
enable_testing()

add_subdirectory(test)

file(COPY test/sample_file.json DESTINATION test)
file(COPY test/complex_file.json DESTINATION test)
