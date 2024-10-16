int invalid_frequency(int freq)
{
    int i=0;

    do
    {
        if (freq == frequencies[i] && freq != 0 )
            return 0;
    } while (frequencies[++i]);
    return 1;
}