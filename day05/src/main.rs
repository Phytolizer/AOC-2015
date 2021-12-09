use itertools::Itertools;

fn main() {
    let input = std::fs::read_to_string("input.txt")
        .unwrap()
        .trim()
        .to_string();

    let mut nnice = 0;
    let mut nnice2 = 0;
    for line in input.lines() {
        if is_nice(line) {
            nnice += 1;
        }
        if is_nice_2(line) {
            nnice2 += 1;
        }
    }
    println!("part 1: {}", nnice);
    println!("part 2: {}", nnice2);
}

fn is_nice(line: &str) -> bool {
    let mut has_double = false;
    let mut has_bad_pair = false;
    let mut nvowels = 0;

    let mut pc = ' ';
    for c in line.trim().chars() {
        if "aeiou".contains(c) {
            nvowels += 1;
        }
        if pc == 'a' && c == 'b'
            || pc == 'c' && c == 'd'
            || pc == 'p' && c == 'q'
            || pc == 'x' && c == 'y'
        {
            has_bad_pair = true;
        }
        if pc == c {
            has_double = true;
        }
        pc = c;
    }

    nvowels >= 3 && has_double && !has_bad_pair
}

fn has_pair(pool: &[char], pair: &[char]) -> bool {
    let mut pc = ' ';
    for c in pool {
        if pc == pair[0] && *c == pair[1] {
            return true;
        }
        pc = *c;
    }
    false
}

fn is_nice_2(line: &str) -> bool {
    let chars = line.trim().chars().collect::<Vec<_>>();
    let mut has_two_pair = false;
    for pair_start_idx in 0..(chars.len() - 1) {
        let pair = &chars[pair_start_idx..=(pair_start_idx + 1)];
        if has_pair(&chars[0..pair_start_idx], pair) || has_pair(&chars[pair_start_idx + 2..], pair)
        {
            has_two_pair = true;
            break;
        }
    }
    let mut has_separated_repetition = false;
    for rep_start_idx in 0..(chars.len() - 2) {
        let rep = &chars[rep_start_idx..=(rep_start_idx + 2)];
        if rep[0] == rep[2] {
            has_separated_repetition = true;
            break;
        }
    }
    has_two_pair && has_separated_repetition
}
