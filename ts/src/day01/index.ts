import assert from "assert";
import { readFileSync } from "fs";
import { join } from "path";

const input = readFileSync(
    join(__dirname, "..", "..", "assets", "day01", "input.txt")
).toString("utf8");

let floor = 0;
let aboveGroundIndex: number | undefined;

for (let i = 0; i < input.length; i++) {
    const c = input[i];
    switch (c) {
        case "(":
            floor++;
            break;
        case ")":
            floor--;
            if (floor < 0 && aboveGroundIndex === undefined) {
                aboveGroundIndex = i + 1;
            }
            break;
        default:
            break;
    }
}

console.log(`Part 1: ${floor}`);
console.log(`Part 2: ${aboveGroundIndex}`);
