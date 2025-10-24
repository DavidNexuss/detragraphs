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

add_library(detragraphs INTERFACE)

target_include_directories(detragraphs
    INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
)

target_include_directories(detragraphs
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/lib/glm
)

# Existing external dependencies
include_directories(lib/detrarandom)
add_subdirectory(lib/detrarandom)

# Existing test executables (now link against the new target)
add_executable(detragraphs_test tests/main.cpp)
target_include_directories(detragraphs_test PRIVATE src)
# Link against the new detragraphs interface target
target_link_libraries(detragraphs_test PRIVATE detragraphs OpenMP::OpenMP_CXX)
target_compile_options(detragraphs_test PRIVATE ${OpenMP_CXX_FLAGS})

add_executable(detragraphs_test_csr tests/main_csr.cpp)
target_include_directories(detragraphs_test_csr PRIVATE src)
target_link_libraries(detragraphs_test_csr PRIVATE detragraphs OpenMP::OpenMP_CXX)
target_compile_options(detragraphs_test_csr PRIVATE ${OpenMP_CXX_FLAGS})

add_executable(detragraphs_test_jacobi tests/main_jacobi.cpp)
target_include_directories(detragraphs_test_jacobi PRIVATE src)
target_link_libraries(detragraphs_test_jacobi PRIVATE detragraphs OpenMP::OpenMP_CXX)
target_compile_options(detragraphs_test_jacobi PRIVATE ${OpenMP_CXX_FLAGS})
