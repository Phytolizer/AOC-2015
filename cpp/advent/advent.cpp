#include "advent.hpp"

#include <fstream>
#include <sstream>

std::string advent::readEntireFile(const std::string& fileName) {
    const std::ifstream file{fileName};
    if (!file.is_open()) {
        throw std::runtime_error{
            "Failed getting puzzle input: Could not open file"};
    }
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}
