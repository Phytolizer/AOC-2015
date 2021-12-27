import itertools
import os
import re


with open(os.path.join(os.path.dirname(os.path.realpath(__file__)), "input.txt")) as f:
    lines = f.readlines()

line_pattern = re.compile(r"(\d+)x(\d+)x(\d+)")

part_1_result = 0
part_2_result = 0

for line in lines:
    match = line_pattern.search(line)
    l, w, h = map(int, match.groups())
    sides = (l * w, w * h, h * l)
    smallest_side = min(sides)
    surface_area = 2 * sum(sides) + smallest_side
    part_1_result += surface_area
    edges = (l, w, h)
    smallest_perimeter = min(
        map(lambda xy: 2 * (xy[0] + xy[1]), itertools.combinations(edges, 2))
    )
    part_2_result += l * w * h + smallest_perimeter

print(f"Part 1: {part_1_result}")
print(f"Part 2: {part_2_result}")
