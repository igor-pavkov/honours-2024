int getfreqcount(int valid)
{
    int i=0, freq_count=0;

    while(G.own_frequencies[i])
    {
        i++;
        if(G.own_frequencies[i] != -1)
            freq_count++;
    }

    if(valid) return freq_count;
    return i;
}