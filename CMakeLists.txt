cmake_minimum_required(VERSION 3.10)

project(detragraphs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ftree-vectorize -march=native")

find_package(OpenMP REQUIRED)

if(OpenMP_CXX_FOUND)
    message(STATUS "OpenMP found: ${OpenMP_CXX_FLAGS}")
endif()

file(GLOB_RECURSE INCLUDE_SRC src/*.cpp)

include_directories(lib/detrarandom)
include_directories(lib/eigen)

add_subdirectory(lib/detrarandom)
add_subdirectory(lib/eigen)

add_executable(detragraphs_test tests/main.cpp)
target_include_directories(detragraphs_test PRIVATE src)
target_link_libraries(detragraphs_test PRIVATE OpenMP::OpenMP_CXX)
target_compile_options(detragraphs_test PRIVATE ${OpenMP_CXX_FLAGS})

add_executable(detragraphs_test_csr tests/main_csr.cpp)
target_include_directories(detragraphs_test_csr PRIVATE src)
target_link_libraries(detragraphs_test_csr PRIVATE OpenMP::OpenMP_CXX)
target_compile_options(detragraphs_test_csr PRIVATE ${OpenMP_CXX_FLAGS})
