cmake_minimum_required(VERSION 3.16)
project(detragraphs LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_COMPILE_WARNING_AS_ERROR OFF)

set(CMAKE_CXX_FLAGS_DEBUG   "-fsanitize=address -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -ftree-vectorize -march=native")

set(USE_OPENMP FALSE)
if(NOT APPLE)
    find_package(OpenMP QUIET)
    if(OpenMP_CXX_FOUND)
        message(STATUS "OpenMP found — binding you tighter")
        set(USE_OPENMP TRUE)
    else()
        message(STATUS "OpenMP eludes us... we proceed alone")
    endif()
else()
    message(STATUS "macOS refuses OpenMP — as always")
endif()


file(GLOB_RECURSE DETRAGRAPHS_SOURCES src/*.cpp)
add_library(detragraphs ${DETRAGRAPHS_SOURCES})

find_package(Eigen3 QUIET NO_MODULE)

if(Eigen3_FOUND)
    get_target_property(EIGEN3_INCLUDE_DIR Eigen3::Eigen INTERFACE_INCLUDE_DIRECTORIES)

    if(EIGEN3_INCLUDE_DIR)
        get_filename_component(EIGEN3_PARENT "${EIGEN3_INCLUDE_DIR}" DIRECTORY)

        target_include_directories(detragraphs
            BEFORE PUBLIC
                "${EIGEN3_PARENT}"
        )

        target_link_libraries(detragraphs PUBLIC Eigen3::Eigen)
    endif()
else()
    include(FetchContent)
    FetchContent_Declare(
        Eigen3
        GIT_REPOSITORY https://gitlab.com/libeigen/eigen.git
        GIT_TAG 3.4.0
        GIT_SHALLOW TRUE
    )
    set(EIGEN_BUILD_DOC OFF CACHE INTERNAL "")
    set(EIGEN_BUILD_PKGCONFIG OFF CACHE INTERNAL "")
    FetchContent_MakeAvailable(Eigen3)

    target_link_libraries(detragraphs PUBLIC Eigen3::Eigen)
endif()

find_package(Spectra QUIET)

if(NOT Spectra_FOUND)
    include(FetchContent)
    FetchContent_Declare(
        Spectra
        GIT_REPOSITORY https://github.com/yixuan/spectra.git
        GIT_TAG master
        GIT_SHALLOW TRUE
    )
    FetchContent_MakeAvailable(Spectra)
endif()

target_link_libraries(detragraphs PUBLIC Spectra::Spectra)

target_include_directories(detragraphs
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        lib/json/include
)

target_link_libraries(detragraphs
    PUBLIC
        Eigen3::Eigen
        Spectra::Spectra   
)

target_link_libraries(detragraphs INTERFACE libscran::umappp)

if(USE_OPENMP)
    target_link_libraries(detragraphs PUBLIC OpenMP::OpenMP_CXX)
    target_compile_options(detragraphs PRIVATE ${OpenMP_CXX_FLAGS})
endif()

include_directories(lib/detrarandom)    # legacy style — consider modernizing later
include_directories(lib/octree)         # legacy style — consider modernizing later
include_directories(lib/umap/include)   # legacy style — consider modernizing later
add_subdirectory(lib/detrarandom)
add_subdirectory(lib/umap)

function(add_detragraphs_test name source_file)
    add_executable(${name} ${source_file})
    target_include_directories(${name} PRIVATE src)
    target_link_libraries(${name} PRIVATE detragraphs)
    if(USE_OPENMP)
        target_link_libraries(${name} PRIVATE OpenMP::OpenMP_CXX)
        target_compile_options(${name} PRIVATE ${OpenMP_CXX_FLAGS})
    endif()
endfunction()

add_detragraphs_test(detragraphs_test            tests/main.cpp)
add_detragraphs_test(detragraphs_test_csr        tests/main_csr.cpp)
add_detragraphs_test(detragraphs_test_jacobi     tests/main_jacobi.cpp)
add_detragraphs_test(detragraphs_test_closeness  tests/main_closeness.cpp)
add_detragraphs_test(detragraphs_test_distance   tests/main_distance.cpp)
