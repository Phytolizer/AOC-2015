use std::collections::HashMap;
use std::collections::HashSet;

use itertools::Itertools;
use once_cell::sync::Lazy;
use regex::Regex;

const INPUT: &str = include_str!("../input.txt");

static LINE_PATTERN: Lazy<Regex> = Lazy::new(|| Regex::new(r"(\w+) to (\w+) = (\d+)").unwrap());

struct Edge {
    a: String,
    b: String,
    dist: usize,
}

#[derive(Debug)]
struct Path {
    distance: usize,
    path: Vec<String>,
}

fn main() {
    let mut nodes = HashSet::<String>::new();
    let mut edges = Vec::<Edge>::new();

    for line in INPUT.lines() {
        let caps = match LINE_PATTERN.captures(line) {
            Some(caps) => caps,
            _ => continue,
        };
        let a = caps.get(1).unwrap().as_str().to_string();
        let b = caps.get(2).unwrap().as_str().to_string();
        let dist = caps.get(3).unwrap().as_str().parse::<usize>().unwrap();
        nodes.insert(a.clone());
        nodes.insert(b.clone());
        edges.push(Edge { a, b, dist });
    }

    let mut g = nodes
        .iter()
        .cloned()
        .map(|n| (n, HashMap::<String, usize>::new()))
        .collect::<HashMap<String, _>>();
    for Edge { a, b, dist } in edges {
        g.get_mut(&a).unwrap().insert(b.clone(), dist);
        g.get_mut(&b).unwrap().insert(a, dist);
    }
    let mut paths = Vec::<Path>::new();
    let nnodes = nodes.len();
    for path in nodes.into_iter().permutations(nnodes) {
        let mut distance = 0;
        for (a, b) in path.iter().tuple_windows() {
            distance += g.get(a).unwrap().get(b).unwrap();
        }
        paths.push(Path { distance, path });
    }
    paths.sort_by(|a, b| a.distance.cmp(&b.distance));
    println!("Shortest path: {:?}", paths[0]);
    println!("Longest path: {:?}", paths.last().unwrap());
}
