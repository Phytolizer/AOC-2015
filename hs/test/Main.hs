module Main where

import Advent (Runner)
import Control.Monad (when)
import Data.Functor ((<&>))
import qualified Dec01
import System.IO (hPrint, stderr)
import Test.HUnit (
    Counts (errors, failures),
    Test (TestCase),
    Testable (test),
    assertEqual,
    runTestTT,
    (~:),
    (~?=),
 )
import Text.Printf (printf)

runner :: Int -> Runner
runner day = case day of
    1 -> Dec01.run

makeTest :: Int -> Int -> Test
makeTest day part = TestCase $ do
    let input = printf "input/dec%02d.txt" day
    let expected = printf "input/dec%02d.expected%d.txt" day part
    actual <- runner day input part
    expected <- readFile expected <&> read
    assertEqual "Test failed" expected actual

tests :: Test
tests =
    test
        [ printf "dec %02d.%d" day part ~: makeTest day part
        | day <- [1]
        , part <-
            [ 1 :: Int
            , 2
            ]
        ]

main :: IO ()
main = do
    counts <- runTestTT tests
    let failed = errors counts + failures counts
    when (failed > 0) $ do
        fail "Tests failed"
