fn look_and_say(text: &str) -> String {
    let mut result = String::new();
    let mut count = 0;
    let mut prev = text.chars().next().unwrap();
    for c in text.chars() {
        if c == prev {
            count += 1
        } else {
            result.push_str(&format!("{}", count));
            result.push(prev);
            prev = c;
            count = 1;
        }
    }
    result.push_str(&format!("{}", count));
    result.push(prev);
    result
}

fn main() {
    let mut s = include_str!("../input.txt").to_string();
    for _ in 0..50 {
        s = look_and_say(&s);
    }
    println!("{}", s.len());
}
