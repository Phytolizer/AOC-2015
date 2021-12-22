import { readFileSync } from "fs";
import { join } from "path";

const input = readFileSync(
    join(__dirname, "..", "..", "assets", "day02", "input.txt")
).toString();

let part1Result = 0;
let part2Result = 0;

input.split("\n").forEach((line) => {
    const [l, w, h] = line.split("x", 3).map((x) => parseInt(x));
    const sides = [l * w, w * h, h * l];
    const smallestSide = Math.min(...sides);
    const surfaceArea =
        sides.map((side) => side * 2).reduce((sum, side) => sum + side, 0) +
        smallestSide;
    part1Result += surfaceArea;

    let minPerimeter: number | undefined;
    const lwh = [l, w, h];
    for (let i = 0; i < 3; i++) {
        for (let j = i + 1; j < 3; j++) {
            const val = lwh[i] * 2 + lwh[j] * 2;
            if (minPerimeter === undefined || minPerimeter > val) {
                minPerimeter = val;
            }
        }
    }
    part2Result += minPerimeter! + l * w * h;
});

console.log(`Part 1: ${part1Result}`);
console.log(`Part 2: ${part2Result}`);
