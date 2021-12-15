use md5::Digest;
use md5::Md5;

const INPUT: &str = include_str!("../input.txt");

fn main() {
    for i in 1.. {
        let input = format!("{}{}", INPUT, i);
        let mut hasher = Md5::new();
        hasher.update(&input);
        let result = hasher.finalize();
        if format!("{:x}", result).starts_with("000000") {
            println!("{}", i);
            break;
        }
    }
}
