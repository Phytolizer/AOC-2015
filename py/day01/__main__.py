import os

with open(os.path.dirname(os.path.realpath(__file__)) + "/input.txt") as f:
    data = f.read().strip()

floor = 0
position = 1
basement_position = 0
for c in data:
    if c == "(":
        floor += 1
    elif c == ")":
        floor -= 1
        if floor < 0 and basement_position == 0:
            basement_position = position
    position += 1

print(f"Part 1: {floor}")
print(f"Part 2: {basement_position}")
