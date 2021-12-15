use std::fs::File;
use std::io::BufRead;
use std::io::BufReader;
use itertools::Itertools;

fn main() {
    let f = BufReader::new(File::open("input.txt").expect("could not open input.txt"));
    let mut result = 0;
    let mut result_2 = 0;
    for line in f.lines() {
        let line = line.expect("could not read line");
        let components = line.split('x').collect::<Vec<&str>>();
        let l = components[0]
            .parse::<u32>()
            .expect("could not parse length");
        let w = components[1].parse::<u32>().expect("could not parse width");
        let h = components[2]
            .parse::<u32>()
            .expect("could not parse height");
        let sides = [l * w, w * h, h * l];
        let smallest_side = *sides.iter().min().unwrap();
        let surface_area = 2 * sides.iter().sum::<u32>() + smallest_side;
        result += surface_area;

        // Part 2
        let smallest_perimeter = [l, w, h]
            .iter()
            .combinations(2)
            .map(|ab| 2 * ab[0] + 2 * ab[1])
            .min()
            .unwrap();
        result_2 += smallest_perimeter + l * w * h;
    }
    println!("Part 1: {}", result);
    println!("Part 2: {}", result_2);
}
