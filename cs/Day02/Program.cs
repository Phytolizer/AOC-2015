using System.Text.RegularExpressions;
using Combinatorics.Collections;
using Helper;

var lines = File.ReadAllLines("Input.txt");
var linePattern = new Regex(@"(\d+)x(\d+)x(\d+)");
var result = 0;
var result2 = 0;
foreach (var line in lines)
{
    var match = linePattern.Match(line);
    var l = int.Parse(match.Groups[1].Value);
    var w = int.Parse(match.Groups[2].Value);
    var h = int.Parse(match.Groups[3].Value);
    var sides = new[]
    {
        l * w,
        w * h,
        h * l
    };
    result += 2 * sides.Sum() + sides.Min();
    result2 += new Combinations<int>(new[] { l, w, h }, 2, GenerateOption.WithoutRepetition)
        .Select(pair => 2 * pair[0] + 2 * pair[1]).Min() + l * w * h;
}

Console.WriteLine($"Part 1: {result}");
Console.WriteLine($"Part 2: {result2}");