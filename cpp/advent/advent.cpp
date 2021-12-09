#include "advent.hpp"
#include <fstream>
#include <sstream>

std::string advent::readEntireFile(const std::string& fileName)
{
    std::ifstream file{fileName};
    std::ostringstream contents;
    contents << file.rdbuf();
    return contents.str();
}
