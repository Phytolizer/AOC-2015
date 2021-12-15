use std::ops::Range;

use itertools::Itertools;
use once_cell::sync::Lazy;
use regex::Regex;

const INPUT: &str = include_str!("../input.txt");

static FORBIDDEN_PATTERN: Lazy<Regex> = Lazy::new(|| Regex::new(r"[ilo]").unwrap());

const CHARSET: Range<u32> = 0..26;

fn increment_password(password: &str) -> String {
    let mut chars = password
        .chars()
        .map(|c| c as u32 - 'a' as u32)
        .collect::<Vec<_>>();
    let mut pos = chars.len() - 1;
    chars[pos] += 1;
    while chars[pos] > CHARSET.end {
        chars[pos] = CHARSET.start;
        if pos == 0 {
            return String::new();
        }
        pos -= 1;
        chars[pos] += 1;
    }
    if chars[pos] > CHARSET.end && pos == 0 {
        chars[pos] = 0;
    }
    chars
        .into_iter()
        .filter_map(|c| char::from_u32(c + 'a' as u32))
        .collect()
}

fn is_valid(password: &str) -> bool {
    has_doubles(password)
        && !FORBIDDEN_PATTERN.is_match(password)
        && has_straight(password)
        && password.len() == 8
}

fn has_doubles(password: &str) -> bool {
    let chars = password.chars().collect::<Vec<_>>();
    match chars.iter().tuple_windows().find_position(|(a, b)| a == b) {
        Some((pos, _)) => {
            pos < chars.len() - 2
                && chars
                    .iter()
                    .skip(pos + 2)
                    .tuple_windows()
                    .any(|(a, b)| a == b)
        }
        None => false,
    }
}

fn has_straight(password: &str) -> bool {
    password.chars().tuple_windows().any(|(a, b, c)| {
        a.is_alphabetic()
            && b.is_alphabetic()
            && c.is_alphabetic()
            && a.to_ascii_lowercase() as u32 + 1 == b.to_ascii_lowercase() as u32
            && b.to_ascii_lowercase() as u32 + 1 == c.to_ascii_lowercase() as u32
    })
}

fn next_password(password: &str) -> String {
    let mut password = increment_password(password);
    while !is_valid(&password) {
        password = increment_password(&password);
    }
    password
}

fn main() {
    let password = next_password(INPUT);
    println!("Part 1: {}", password);
    println!("Part 2: {}", next_password(&password));
}
