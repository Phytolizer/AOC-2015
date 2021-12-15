namespace Helper;

public static class EnumerableUtils
{
    public static IEnumerable<(T, T)> GetUniquePairs<T>(IEnumerable<T> items)
    {
        var itemsArray = items as T[] ?? items.ToArray();
        for (var index = 0; index < itemsArray.Length; index++)
        {
            var item = itemsArray[index];
            foreach (var result in itemsArray.Skip(index + 1))
            {
                yield return (item, result);
            }
        }
    }
}