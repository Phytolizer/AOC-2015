fn main() {
    let input = std::fs::read_to_string("input.txt").unwrap();
    let mut counter = 0;
    for (i, c) in input.char_indices() {
        match c {
            '(' => counter += 1,
            ')' => {
                counter -= 1;
                if counter < 0 {
                    println!("{}", i + 1);
                    break;
                }
            }
            _ => (),
        }
    }
}
