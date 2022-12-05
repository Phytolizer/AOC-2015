module Dec01 where

import Advent (Runner)
import Control.Monad (join)
import Data.Char (isSpace)
import Data.Function ((&))
import Data.Functor ((<&>))
import System.IO (Handle, IOMode (ReadMode), hGetContents, openFile)

openFile' :: IOMode -> FilePath -> IO Handle
openFile' mode path = openFile path mode

solvePart2 :: String -> Int -> Int -> Int
solvePart2 [] _ _ = 0
solvePart2 (x : xs) pos n =
    if pos < 0
        then n
        else case x of
            '(' -> solvePart2 xs (pos + 1) (n + 1)
            ')' -> solvePart2 xs (pos - 1) (n + 1)
            _ -> solvePart2 xs pos n

solve :: Int -> String -> Int
solve _ [] = 0
solve 1 text = case text of
    '(' : rest -> 1 + solve 1 rest
    ')' : rest -> -1 + solve 1 rest
    (c : rest) | isSpace c -> solve 1 rest
    _ -> error "Invalid input"
solve 2 text = solvePart2 text 0 0

run :: Runner
run input part =
    input
        & openFile' ReadMode
        <&> hGetContents
        & join
        <&> solve part
