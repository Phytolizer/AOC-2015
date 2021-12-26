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
target_link_libraries("advent" PUBLIC "fmt::fmt")

function(advent TARGET)
    cmake_parse_arguments(
        PARSE_ARGV
        0
        "ADVENT"
        ""
        ""
        "LIBS"
    )
    add_executable("${TARGET}" "${TARGET}/main.cpp")
    target_link_libraries("${TARGET}" PRIVATE "advent" ${ADVENT_LIBS})
    set_target_properties("${TARGET}" PROPERTIES OUTPUT_NAME "${TARGET}_bin")
    configure_file(
        "config.hpp.cin" "${PROJECT_BINARY_DIR}/${TARGET}/config.hpp"
    )
    target_include_directories(
        "${TARGET}" PUBLIC "${PROJECT_BINARY_DIR}/${TARGET}"
    )
endfunction()

advent("day01")
advent(
    "day02"
    LIBS
    "ctre::ctre"
)
advent("day03")
advent(
    "day04"
    LIBS
    "crypto"
)
advent("day05")
advent(
    "day06"
    LIBS
    "ctre::ctre"
)
advent(
    "day07"
    LIBS
    "ctre::ctre"
)
advent(
    "day08"
    LIBS
    "ctre::ctre"
)
