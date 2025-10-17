cmake_minimum_required(VERSION 3.10)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ftree-vectorize -march=native -fopenmp")
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(DOXYGEN_GENERATE_TREEVIEW YES)

project(detragraphs)

find_package(OpenMP)

file(GLOB_RECURSE SRC 
  src/*.cpp
)

file(GLOB_RECURSE INCLUDE_SRC
  detragraphs/*.cpp
)

include_directories(lib/detrarandom)

add_subdirectory(lib/detrarandom)

# add_library(detragraphs ${INCLUDE_SRC})
# target_link_libraries(detragraphs detrarandom)

add_executable(detragraphs_test ${SRC})

target_include_directories(detragraphs_test PRIVATE detragraphs)
# target_link_libraries(detragraphs_test detragraphs)
