cmake_minimum_required(VERSION 3.10)

project(detragraphs)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

set(CMAKE_CXX_FLAGS_DEBUG "-fsanitize=address -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ftree-vectorize -march=native")

# --------------------------
# OpenMP Configuration
# --------------------------
# Only attempt to find and use OpenMP if NOT on macOS (APPLE).
if(NOT APPLE)
    find_package(OpenMP) # No longer REQUIRED
    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found: ${OpenMP_CXX_FLAGS}")
        set(USE_OPENMP TRUE)
    else()
        message(STATUS "OpenMP not found. Building without OpenMP support.")
    endif()
else()
    message(STATUS "Skipping OpenMP on macOS.")
endif()

# --------------------------
# Add library
# --------------------------
file(GLOB_RECURSE DETRAGRAPHS_SOURCES src/*.cpp)
add_library(detragraphs ${DETRAGRAPHS_SOURCES})
target_include_directories(detragraphs PUBLIC src)
target_include_directories(detragraphs PUBLIC lib/json/include)

if(USE_OPENMP)
    target_link_libraries(detragraphs PUBLIC OpenMP::OpenMP_CXX)
    target_compile_options(detragraphs PRIVATE ${OpenMP_CXX_FLAGS})
endif()

# --------------------------
# External dependencies
# --------------------------
include_directories(lib/detrarandom)
add_subdirectory(lib/detrarandom)

# --------------------------
# Tests
# --------------------------
add_executable(detragraphs_test tests/main.cpp)
target_include_directories(detragraphs_test PRIVATE src)
target_link_libraries(detragraphs_test PRIVATE detragraphs) 
if(USE_OPENMP)
    target_link_libraries(detragraphs_test PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(detragraphs_test PRIVATE ${OpenMP_CXX_FLAGS})
endif()

add_executable(detragraphs_test_csr tests/main_csr.cpp)
target_include_directories(detragraphs_test_csr PRIVATE src)
target_link_libraries(detragraphs_test_csr PRIVATE detragraphs)
if(USE_OPENMP)
    target_link_libraries(detragraphs_test_csr PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(detragraphs_test_csr PRIVATE ${OpenMP_CXX_FLAGS})
endif()

add_executable(detragraphs_test_jacobi tests/main_jacobi.cpp)
target_include_directories(detragraphs_test_jacobi PRIVATE src)
target_link_libraries(detragraphs_test_jacobi PRIVATE detragraphs)
if(USE_OPENMP)
    target_link_libraries(detragraphs_test_jacobi PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(detragraphs_test_jacobi PRIVATE ${OpenMP_CXX_FLAGS})
endif()

add_executable(detragraphs_test_distance tests/main_distance.cpp)
target_include_directories(detragraphs_test_distance PRIVATE src)
target_link_libraries(detragraphs_test_distance PRIVATE detragraphs)
if(USE_OPENMP)
    target_link_libraries(detragraphs_test_distance PRIVATE OpenMP::OpenMP_CXX)
    target_compile_options(detragraphs_test_distance PRIVATE ${OpenMP_CXX_FLAGS})
endif()
