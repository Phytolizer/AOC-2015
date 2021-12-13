use clap::App;
use clap::Arg;
use logos::Lexer;
use logos::Logos;
use std::fs::File;
use std::io::BufRead;
use std::io::BufReader;

fn get_escape_value(lex: &mut Lexer<TokenKind>) -> char {
    let slice = lex.slice();
    // \xHH
    u8::from_str_radix(&slice[2..], 16).unwrap() as char
}

fn get_second_char(lex: &mut Lexer<TokenKind>) -> char {
    let slice = lex.slice();
    slice.chars().nth(1).unwrap()
}

fn get_only_char(lex: &mut Lexer<TokenKind>) -> char {
    let slice = lex.slice();
    slice.chars().next().unwrap()
}

#[derive(Debug, Logos)]
enum TokenKind {
    #[token(r#"""#)]
    BareQuote,

    #[regex(r"\\x([0-9a-fA-F][0-9a-fA-F])", get_escape_value)]
    #[regex(r"\\\\", get_second_char)]
    #[regex(r#"\\""#, get_second_char)]
    Escape(char),

    #[regex(r".", get_only_char)]
    Char(char),

    #[error]
    Error,
}

#[derive(Debug, PartialEq)]
enum ParseState {
    Begin,
    Normal,
    End,
}

struct DecodedChar {
    value: char,
    is_hexcoded: bool,
}

fn main() {
    let mut digital_size = 0;
    let mut physical_size = 0;
    let mut encoded_size = 0;

    let matches = App::new("day08")
        .arg(
            Arg::with_name("file")
                .short("f")
                .long("file")
                .takes_value(true),
        )
        .get_matches();
    let file = matches.value_of("file").unwrap_or("input.txt");

    for (line_num, line) in BufReader::new(File::open(file).unwrap())
        .lines()
        .enumerate()
    {
        let line = line.unwrap();

        physical_size += line.chars().count();
        let mut chars = Vec::new();
        let mut state = ParseState::Begin;
        for token in TokenKind::lexer(&line) {
            match token {
                TokenKind::BareQuote => match state {
                    ParseState::Begin => state = ParseState::Normal,
                    ParseState::Normal => state = ParseState::End,
                    ParseState::End => panic!(
                        "[line {}] Could not parse string: Too many bare quotes",
                        line_num
                    ),
                },
                TokenKind::Char(c) => match state {
                    ParseState::Begin => panic!(
                        "[line {}] Could not parse string: Leading characters",
                        line_num
                    ),
                    ParseState::Normal => chars.push(DecodedChar {
                        value: c,
                        is_hexcoded: false,
                    }),
                    ParseState::End => panic!(
                        "[line {}] Could not parse string: Trailing characters",
                        line_num
                    ),
                },
                TokenKind::Escape(c) => match state {
                    ParseState::Begin => {
                        panic!("[line {}] Could not parse string: Leading escape", line_num)
                    }
                    ParseState::Normal => chars.push(DecodedChar {
                        value: c,
                        is_hexcoded: true,
                    }),
                    ParseState::End => panic!(
                        "[line {}] Could not parse string: Trailing escape",
                        line_num
                    ),
                },
                TokenKind::Error => unreachable!(),
            }
        }
        assert!(
            state == ParseState::End,
            "[line {}] Could not parse string: Incomplete string",
            line_num
        );
        digital_size += chars.len();

        let mut encoded_str = r#""\""#.to_string();
        for DecodedChar { value, is_hexcoded } in chars {
            if value == '"' {
                encoded_str.push_str(r#"\\\""#);
            } else if is_hexcoded {
                encoded_str.push_str(r"\\x");
                encoded_str.push_str(&format!("{:02x}", value as u8));
            } else {
                encoded_str.push(value);
            }
        }
        encoded_str.push_str(r#"\"""#);
        println!("{}", encoded_str);
        encoded_size += encoded_str.chars().count();
    }

    println!("Digital size: {}", digital_size);
    println!("Physical size: {}", physical_size);
    println!("Part 1: {}", physical_size - digital_size);
    println!("Part 2: {}", encoded_size - physical_size);
}
