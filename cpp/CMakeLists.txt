cmake_minimum_required(VERSION 3.12...3.22)

project(
    "Advent of Code 2015"
    VERSION "2015.0.0"
    LANGUAGES "CXX"
    HOMEPAGE_URL "https://github.com/Phytolizer/aoc-2015"
)

add_library("advent" "advent/advent.cpp" "advent/inc/advent.hpp")
target_include_directories("advent" PUBLIC "advent/inc")

add_executable("day01" "day01/main.cpp")
target_link_libraries("day01" PRIVATE "advent")
