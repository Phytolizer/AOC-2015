use std::collections::HashSet;

enum Mover {
    Santa,
    RoboSanta,
}

fn main() {
    let mut coords = (0, 0);
    let mut santa = (0, 0);
    let mut robo_santa = (0, 0);
    let mut mover = Mover::Santa;
    let mut visited = HashSet::<(i32, i32)>::new();
    visited.insert(santa);
    let mut nvisited = 1;
    let mut visited_2 = HashSet::<(i32, i32)>::new();
    let mut nvisited_2 = 0;
    let text = std::fs::read_to_string("input.txt").unwrap();
    for c in text.chars() {
        let mx = match mover {
            Mover::Santa => {
                mover = Mover::RoboSanta;
                &mut santa
            }
            Mover::RoboSanta => {
                mover = Mover::Santa;
                &mut robo_santa
            }
        };
        match c {
            '^' => {
                mx.0 += 1;
                coords.0 += 1;
            }
            'v' => {
                mx.0 -= 1;
                coords.0 -= 1;
            }
            '<' => {
                mx.1 -= 1;
                coords.1 -= 1;
            }
            '>' => {
                mx.1 += 1;
                coords.1 += 1;
            }
            _ => (),
        }
        if !visited.contains(&coords) {
            visited.insert(coords);
            nvisited += 1;
        }
        if !visited_2.contains(mx) {
            visited_2.insert(*mx);
            nvisited_2 += 1;
        }
    }
    println!("Part 1: {}", nvisited);
    println!("Part 2: {}", nvisited_2);
}
