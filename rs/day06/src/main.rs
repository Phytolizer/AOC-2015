use std::fs::File;
use std::io::BufRead;
use std::io::BufReader;

use once_cell::sync::Lazy;
use regex::Regex;

static REGEX: Lazy<Regex> = Lazy::new(|| {
    Regex::new(r"(?P<action>turn on|turn off|toggle) (?P<tlx>\d+),(?P<tly>\d+) through (?P<brx>\d+),(?P<bry>\d+)").unwrap()
});

enum Action {
    TurnOn,
    TurnOff,
    Toggle,
}

fn main() {
    let mut grid = vec![vec![false; 1000]; 1000];
    let mut grid2 = vec![vec![0usize; 1000]; 1000];
    for line in BufReader::new(File::open("input.txt").unwrap()).lines() {
        let line = line.unwrap();
        debug_assert!(REGEX.is_match(&line));
        let caps = REGEX.captures(&line).unwrap();
        let action = match caps.name("action").unwrap().as_str() {
            "turn on" => Action::TurnOn,
            "turn off" => Action::TurnOff,
            "toggle" => Action::Toggle,
            _ => unreachable!(),
        };
        let tlx = caps.name("tlx").unwrap().as_str().parse::<usize>().unwrap();
        let tly = caps.name("tly").unwrap().as_str().parse::<usize>().unwrap();
        let brx = caps.name("brx").unwrap().as_str().parse::<usize>().unwrap();
        let bry = caps.name("bry").unwrap().as_str().parse::<usize>().unwrap();

        for row in grid.iter_mut().take(brx + 1).skip(tlx) {
            for light in row.iter_mut().take(bry + 1).skip(tly) {
                match action {
                    Action::TurnOn => *light = true,
                    Action::TurnOff => *light = false,
                    Action::Toggle => *light = !*light,
                }
            }
        }

        for row in grid2.iter_mut().take(brx + 1).skip(tlx) {
            for light in row.iter_mut().take(bry + 1).skip(tly) {
                match action {
                    Action::TurnOn => *light += 1,
                    Action::TurnOff => {
                        if *light > 0 {
                            *light -= 1;
                        }
                    }
                    Action::Toggle => *light += 2,
                }
            }
        }
    }

    println!(
        "part 1: {}",
        grid.iter()
            .map(|row| row.iter().filter(|light| **light).count())
            .sum::<usize>()
    );
    println!(
        "part 2: {}",
        grid2
            .iter()
            .map(|row| row.iter().sum::<usize>())
            .sum::<usize>()
    );
}
