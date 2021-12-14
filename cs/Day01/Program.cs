using System;

var input = File.ReadAllText("Input.txt");
var floor = 0;
var index = 1;
var indexFound = false;
foreach (var c in input)
{
    switch (c)
    {
        case '(':
            ++floor;
            break;
        case ')':
            --floor;
            if (!indexFound && floor < 0)
            {
                Console.WriteLine($"Part 2: {index}");
                indexFound = true;
            }

            break;
    }

    ++index;
}

Console.WriteLine($"Part 1: {floor}");