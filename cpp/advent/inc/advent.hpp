#pragma once

#include <string>

#define ADVENT_FOREACH_CHAR(File, CharVar) while ((File) >> (CharVar))

namespace advent
{
std::string readEntireFile(const std::string& fileName);
}
