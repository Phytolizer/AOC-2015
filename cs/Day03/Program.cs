using Day03;

var input = File.ReadAllText("Input.txt");
var part1Coords = new Mover();
var santa = new Mover();
var roboSanta = new Mover();
var mover = WhichMover.Santa;
var visited = new HashSet<(int, int)> { (part1Coords.X, part1Coords.Y) };
var visited2 = new HashSet<(int, int)> { (santa.X, santa.Y) };

foreach (var c in input.Where(c => "^v<>".Contains(c)))
{
    Mover mx;
    switch (mover)
    {
        case WhichMover.Santa:
            mx = santa;
            mover = WhichMover.RoboSanta;
            break;
        case WhichMover.RoboSanta:
            mx = roboSanta;
            mover = WhichMover.Santa;
            break;
        default:
            throw new ArgumentOutOfRangeException();
    }

    switch (c)
    {
        case '^':
            mx.Move(0, -1);
            part1Coords.Move(0, -1);
            break;
        case 'v':
            mx.Move(0, 1);
            part1Coords.Move(0, 1);
            break;
        case '<':
            mx.Move(-1, 0);
            part1Coords.Move(-1, 0);
            break;
        case '>':
            mx.Move(1, 0);
            part1Coords.Move(1, 0);
            break;
    }

    visited.Add((part1Coords.X, part1Coords.Y));
    visited2.Add((mx.X, mx.Y));
}

Console.WriteLine($"Part 1: {visited.Count}");
Console.WriteLine($"Part 2: {visited2.Count}");