cmake_minimum_required(VERSION 3.12...3.22)

project(
    "Advent of Code 2015"
    VERSION "2015.0.0"
    LANGUAGES "CXX"
    HOMEPAGE_URL "https://github.com/Phytolizer/aoc-2015"
)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)
FetchContent_Declare(
    "fmt"
    GIT_REPOSITORY "https://github.com/fmtlib/fmt.git"
    GIT_TAG "8.0.1"
)
FetchContent_MakeAvailable("fmt")
FetchContent_Declare(
    "ctre"
    GIT_REPOSITORY
        "https://github.com/hanickadot/compile-time-regular-expressions.git"
    GIT_TAG "v3.4.1"
)
FetchContent_MakeAvailable("ctre")

add_library(
    "advent"
    "advent/advent.cpp"
    "advent/inc/advent.hpp"
    "cpp.hint"
)
target_include_directories("advent" PUBLIC "advent/inc")
target_link_libraries("advent" PUBLIC "fmt::fmt" "ctre::ctre")

macro(copy_puzzle_input TARGET)
    add_custom_target(
        "${TARGET}_input"
        DEPENDS "${TARGET}/input.txt"
        COMMAND "${CMAKE_COMMAND}" -E make_directory
                "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}"
        COMMAND
            "${CMAKE_COMMAND}" -E copy
            "${PROJECT_SOURCE_DIR}/${TARGET}/input.txt"
            "${CMAKE_CURRENT_BINARY_DIR}/${TARGET}/input.txt"
    )
    add_dependencies("${TARGET}" "${TARGET}_input")
endmacro()

macro(advent TARGET)
add_executable("${TARGET}" "${TARGET}/main.cpp" ${ARGN})
target_link_libraries("${TARGET}" PRIVATE "advent")
set_target_properties("${TARGET}" PROPERTIES OUTPUT_NAME "${TARGET}_bin")
copy_puzzle_input("${TARGET}")
endmacro()

advent("day01")
advent("day02")
advent("day03")
