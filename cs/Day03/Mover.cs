namespace Day03;

internal class Mover
{
    public int X { get; set; }
    public int Y { get; set; }

    public override int GetHashCode()
    {
        return (X, Y).GetHashCode();
    }

    public void Move(int dx, int dy)
    {
        X += dx;
        Y += dy;
    }
}