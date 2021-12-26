#include <advent.hpp>
#include <array>
#include <ctre.hpp>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

enum class Action {
  TURN_ON,
  TURN_OFF,
  TOGGLE,
};

size_t parse(std::string_view str) {
  size_t result;
  std::istringstream ss{std::string{str}};
  ss >> result;
  return result;
}

int main() {
  std::ifstream input{"day06/input.txt"};

  auto gridMem = std::make_unique<std::array<std::array<bool, 1000>, 1000>>();
  auto& grid = *gridMem;
  auto grid2Mem =
      std::make_unique<std::array<std::array<size_t, 1000>, 1000>>();
  auto& grid2 = *grid2Mem;
  std::string line;
  ADVENT_FOREACH_LINE(input, line) {
    ctre::regex_results match = ctre::match<
        R"((?<action>turn on|turn off|toggle) (?<tlx>\d+),(?<tly>\d+) through (?<brx>\d+),(?<bry>\d+))">(
        line);
    std::string_view rawAction = match.get<"action">();
    Action action;
    if (rawAction == "turn on") {
      action = Action::TURN_ON;
    } else if (rawAction == "turn off") {
      action = Action::TURN_OFF;
    } else {
      action = Action::TOGGLE;
    }
    std::string_view tlxStr = match.get<"tlx">();
    std::string_view tlyStr = match.get<"tly">();
    std::string_view brxStr = match.get<"brx">();
    std::string_view bryStr = match.get<"bry">();
    size_t tlx = parse(tlxStr);
    size_t tly = parse(tlyStr);
    size_t brx = parse(brxStr);
    size_t bry = parse(bryStr);
    for (size_t y = tly; y <= bry; ++y) {
      for (size_t x = tlx; x <= brx; ++x) {
        switch (action) {
          case Action::TURN_ON:
            grid[y][x] = true;
            grid2[y][x] += 1;
            break;
          case Action::TURN_OFF:
            grid[y][x] = false;
            if (grid2[y][x] > 0) {
              grid2[y][x] -= 1;
            }
            break;
          case Action::TOGGLE:
            grid[y][x] = !grid[y][x];
            grid2[y][x] += 2;
            break;
        }
      }
    }
  }

  size_t part1 = 0;
  for (const auto& row : grid) {
    for (auto cell : row) {
      if (cell) {
        ++part1;
      }
    }
  }
  std::cout << "Part 1: " << part1 << "\n";
  size_t part2 = 0;
  for (const auto& row : grid2) {
    for (auto cell : row) {
      part2 += cell;
    }
  }
  std::cout << "Part 2: " << part2 << "\n";
}
