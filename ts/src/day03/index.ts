import { readFileSync } from "fs";
import { join } from "path";

const input = readFileSync(
    join(__dirname, "..", "..", "assets", "day03", "input.txt")
).toString();

let part1Santa: [number, number] = [0, 0];
let santa: [number, number] = [0, 0];
let roboSanta: [number, number] = [0, 0];

let part1Houses = new Set<string>();
part1Houses.add("0,0");

for (const c of input) {
    switch (c) {
        case "<":
            part1Santa[0]--;
            part1Houses.add(`${part1Santa[0]},${part1Santa[1]}`);
            break;
        case ">":
            part1Santa[0]++;
            part1Houses.add(`${part1Santa[0]},${part1Santa[1]}`);
            break;
        case "^":
            part1Santa[1]--;
            part1Houses.add(`${part1Santa[0]},${part1Santa[1]}`);
            break;
        case "v":
            part1Santa[1]++;
            part1Houses.add(`${part1Santa[0]},${part1Santa[1]}`);
            break;
        default:
            break;
    }
}

console.log(`Part 1: ${part1Houses.size}`);
