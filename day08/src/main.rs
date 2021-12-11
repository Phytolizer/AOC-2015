use logos::Lexer;
use logos::Logos;
use std::fs::File;
use std::io::BufRead;
use std::io::BufReader;
use std::num::ParseIntError;

fn get_escape_value(lex: &mut Lexer<TokenKind>) -> Result<char, ParseIntError> {
    let slice = lex.slice();
    // \xHH
    Ok(u8::from_str_radix(&slice[2..], 16)? as char)
}

fn get_second_char(lex: &mut Lexer<TokenKind>) -> char {
    let slice = lex.slice();
    slice.chars().nth(1).unwrap()
}

fn get_only_char(lex: &mut Lexer<TokenKind>) -> char {
    let slice = lex.slice();
    slice.chars().next().unwrap()
}

#[derive(Debug, logos::Logos)]
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

fn main() {
    let mut digital_size = 0;
    let mut physical_size = 0;

    for line in BufReader::new(File::open("input.txt").unwrap()).lines() {
        let line = line.unwrap();

        physical_size += line.len();
        let mut chars = Vec::new();
        let mut state = ParseState::Begin;
        for token in TokenKind::lexer(&line) {
            match token {
                TokenKind::BareQuote => match state {
                    ParseState::Begin => state = ParseState::Normal,
                    ParseState::Normal => state = ParseState::End,
                    ParseState::End => panic!("Could not parse string: Too many bare quotes"),
                },
                TokenKind::Char(c) => match state {
                    ParseState::Begin => panic!("Could not parse string: Leading characters"),
                    ParseState::Normal => chars.push(c),
                    ParseState::End => panic!("Could not parse string: Trailing characters"),
                },
                TokenKind::Escape(c) => match state {
                    ParseState::Begin => panic!("Could not parse string: Leading escape"),
                    ParseState::Normal => chars.push(c),
                    ParseState::End => panic!("Could not parse string: Trailing escape"),
                },
                TokenKind::Error => panic!("Could not parse string: Invalid token"),
            }
        }
        assert!(
            state == ParseState::End,
            "Could not parse string: Incomplete string"
        );
        digital_size += chars.len();
    }

    println!("Digital size: {}", digital_size);
    println!("Physical size: {}", physical_size);
    println!("Part 1: {}", physical_size - digital_size);

}
