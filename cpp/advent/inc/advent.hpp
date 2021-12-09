#pragma once

#include <string>

#define ADVENT_FOREACH_CHAR(File, CharVar) while ((File) >> (CharVar))
#define ADVENT_FOREACH_LINE(File, LineVar) while (std::getline((File), (LineVar)))

namespace advent
{
std::string readEntireFile(const std::string& fileName);
}
