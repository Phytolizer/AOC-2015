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

add_executable("day01" "day01/main.cpp")
target_link_libraries("day01" PRIVATE "advent")
copy_puzzle_input("day01")

add_executable("day02" "day02/main.cpp")
target_link_libraries("day02" PRIVATE "advent")
copy_puzzle_input("day02")
