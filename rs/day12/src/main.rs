use json::JsonValue;

const INPUT: &str = include_str!("../input.txt");

fn sum_json(json: &JsonValue, sum: &mut i64) {
    match json {
        JsonValue::Number(n) => *sum += n.as_fixed_point_i64(0).unwrap() as i64,
        JsonValue::Object(o) => o.iter().for_each(|(_, v)| sum_json(v, sum)),
        JsonValue::Array(a) => a.iter().for_each(|j| sum_json(j, sum)),
        _ => {}
    }
}

fn sum_non_red_json(json: &JsonValue, sum: &mut i64) {
    match json {
        JsonValue::Number(n) => *sum += n.as_fixed_point_i64(0).unwrap() as i64,
        JsonValue::Object(o) => {
            if o.iter().any(|(_, v)| is_red(v)) {
                return;
            }
            for (_, v) in o.iter() {
                sum_non_red_json(v, sum);
            }
        }
        JsonValue::Array(a) => a.iter().for_each(|j| sum_non_red_json(j, sum)),
        _ => {}
    }
}

fn is_red(v: &JsonValue) -> bool {
    match v {
        JsonValue::Short(s) => s.as_str() == "red",
        JsonValue::String(s) => s == "red",
        _ => false,
    }
}

fn main() {
    let input = json::parse(INPUT).unwrap();
    let mut sum = 0;
    sum_json(&input, &mut sum);
    println!("Part 1: {}", sum);
    let mut sum2 = 0;
    sum_non_red_json(&input, &mut sum2);
    println!("Part 2: {}", sum2);
}
