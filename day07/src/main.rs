use once_cell::sync::Lazy;
use regex::Regex;
use std::collections::HashMap;
use std::fmt::Display;

const DEBUG_OUTPUT: bool = false;

static WIRE_PATTERN: Lazy<Regex> = Lazy::new(|| Regex::new(r"^[a-z]+$").unwrap());

type SignalStrength = u16;

#[derive(Debug, Clone, Copy)]
enum Op {
    Set,
    Not,
    Rshift,
    Lshift,
    And,
    Or,
}

impl Display for Op {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", format!("{:?}", self).to_uppercase())
    }
}

struct Gate {
    name: String,
    op: Op,
    input_a: Option<String>,
    input_b: Option<String>,
    description: String,
    value1: Option<SignalStrength>,
    bits: Option<SignalStrength>,
}

impl Gate {
    fn new(name: String, op: Op, input_a: Option<String>, input_b: Option<String>) -> Gate {
        let description = match input_a {
            Some(ref a) => match input_b {
                Some(ref b) => format!("{} {} {}", a, op, b),
                _ => a.to_string(),
            },
            None => format!("{} {}", op, input_b.as_ref().unwrap()),
        };
        let mut g = Gate {
            name,
            op,
            input_a,
            input_b,
            description,
            value1: None,
            bits: None,
        };
        match op {
            Op::Set => {
                assert!(g.input_b.is_none());
                if !WIRE_PATTERN.is_match(g.input_a.as_ref().unwrap()) {
                    g.value1 = Some(g.input_a.as_ref().unwrap().parse().unwrap());
                }
            }
            Op::Lshift | Op::Rshift => {
                assert!(WIRE_PATTERN.is_match(g.input_a.as_ref().unwrap()));
                g.bits = Some(g.input_b.as_ref().unwrap().parse().unwrap());
            }
            Op::Not => {
                assert!(g.input_a.is_none());
                assert!(WIRE_PATTERN.is_match(g.input_b.as_ref().unwrap()));
            }
            Op::And | Op::Or => {
                assert!(WIRE_PATTERN.is_match(g.input_b.as_ref().unwrap()));
                if !WIRE_PATTERN.is_match(g.input_a.as_ref().unwrap()) {
                    g.value1 = Some(g.input_a.as_ref().unwrap().parse().unwrap());
                }
            }
        }
        g
    }

    fn eval(
        &self,
        gates: &HashMap<String, Gate>,
        cache: &mut HashMap<String, SignalStrength>,
        depth: usize,
    ) -> SignalStrength {
        if DEBUG_OUTPUT {
            println!(
                "{:indent$}Evaluating {} => {}",
                "",
                self.name,
                self.description,
                indent = depth * 2
            );
        }
        if cache.get(&self.name).is_some() {
            if DEBUG_OUTPUT {
                println!(
                    "{:indent$}=> {} (cached)",
                    "",
                    cache.get(&self.name).unwrap(),
                    indent = depth * 2
                );
            }
            return *cache.get(&self.name).unwrap();
        }
        match self.op {
            Op::Set => self.value1.unwrap_or_else(|| {
                let result = gates.get(self.input_a.as_ref().unwrap()).unwrap().eval(
                    gates,
                    cache,
                    depth + 1,
                );
                cache.insert(self.name.clone(), result);
                if DEBUG_OUTPUT {
                    println!("{:indent$}=> {}", "", result, indent = depth * 2);
                }
                result
            }),
            Op::Not => {
                let result = !gates.get(self.input_b.as_ref().unwrap()).unwrap().eval(
                    gates,
                    cache,
                    depth + 1,
                );
                cache.insert(self.name.clone(), result);
                if DEBUG_OUTPUT {
                    println!("{:indent$}=> {}", "", result, indent = depth * 2);
                }
                result
            }
            Op::Rshift => {
                let result = gates.get(self.input_a.as_ref().unwrap()).unwrap().eval(
                    gates,
                    cache,
                    depth + 1,
                ) >> self.bits.unwrap();
                cache.insert(self.name.clone(), result);
                if DEBUG_OUTPUT {
                    println!("{:indent$}=> {}", "", result, indent = depth * 2);
                }
                result
            }
            Op::Lshift => {
                let result = gates.get(self.input_a.as_ref().unwrap()).unwrap().eval(
                    gates,
                    cache,
                    depth + 1,
                ) << self.bits.unwrap();
                cache.insert(self.name.clone(), result);
                if DEBUG_OUTPUT {
                    println!("{:indent$}=> {}", "", result, indent = depth * 2);
                }
                result
            }
            Op::And => match self.value1 {
                Some(v) => {
                    let result = v & gates.get(self.input_b.as_ref().unwrap()).unwrap().eval(
                        gates,
                        cache,
                        depth + 1,
                    );
                    cache.insert(self.name.clone(), result);
                    if DEBUG_OUTPUT {
                        println!("{:indent$}=> {}", "", result, indent = depth * 2);
                    }
                    result
                }
                None => {
                    let result = gates.get(self.input_a.as_ref().unwrap()).unwrap().eval(
                        gates,
                        cache,
                        depth + 1,
                    ) & gates.get(self.input_b.as_ref().unwrap()).unwrap().eval(
                        gates,
                        cache,
                        depth + 1,
                    );
                    cache.insert(self.name.clone(), result);
                    if DEBUG_OUTPUT {
                        println!("{:indent$}=> {}", "", result, indent = depth * 2);
                    }
                    result
                }
            },
            Op::Or => match self.value1 {
                Some(v) => {
                    let result = v | gates.get(self.input_b.as_ref().unwrap()).unwrap().eval(
                        gates,
                        cache,
                        depth + 1,
                    );
                    cache.insert(self.name.clone(), result);
                    if DEBUG_OUTPUT {
                        println!("{:indent$}=> {}", "", result, indent = depth * 2);
                    }
                    result
                }
                None => {
                    let result = gates.get(self.input_a.as_ref().unwrap()).unwrap().eval(
                        gates,
                        cache,
                        depth + 1,
                    ) | gates.get(self.input_b.as_ref().unwrap()).unwrap().eval(
                        gates,
                        cache,
                        depth + 1,
                    );
                    cache.insert(self.name.clone(), result);
                    if DEBUG_OUTPUT {
                        println!("{:indent$}=> {}", "", result, indent = depth * 2);
                    }
                    result
                }
            },
        }
    }
}

fn parse_line(line: &str) -> Option<Gate> {
    let (inputs, name) = match line.split_once(" -> ") {
        Some(parts) => parts,
        None => return None,
    };
    let words = inputs.split_whitespace().collect::<Vec<_>>();
    match words.len() {
        1 => Some(Gate::new(
            name.to_string(),
            Op::Set,
            Some(words[0].to_string()),
            None,
        )),
        2 => Some(Gate::new(
            name.to_string(),
            match words[0] {
                "NOT" => Op::Not,
                "RSHIFT" => Op::Rshift,
                "LSHIFT" => Op::Lshift,
                "AND" => Op::And,
                "OR" => Op::Or,
                _ => return None,
            },
            None,
            Some(words[1].to_string()),
        )),
        3 => Some(Gate::new(
            name.to_string(),
            match words[1] {
                "NOT" => Op::Not,
                "RSHIFT" => Op::Rshift,
                "LSHIFT" => Op::Lshift,
                "AND" => Op::And,
                "OR" => Op::Or,
                _ => return None,
            },
            Some(words[0].to_string()),
            Some(words[2].to_string()),
        )),
        _ => None,
    }
}

fn main() {
    let input = include_str!("../input.txt");
    let mut gates = HashMap::new();
    let mut cache = HashMap::new();
    for line in input.lines() {
        if let Some(gate) = parse_line(line) {
            gates.insert(gate.name.clone(), gate);
        }
    }

    let a_value = gates.get("a").unwrap().eval(&gates, &mut cache, 0);
    println!("Part 1: {}", a_value);

    gates.insert(
        "b".to_string(),
        Gate::new("b".to_string(), Op::Set, Some(a_value.to_string()), None),
    );
    cache.clear();
    println!(
        "Part 2: {}",
        gates.get("a").unwrap().eval(&gates, &mut cache, 0)
    );
}
