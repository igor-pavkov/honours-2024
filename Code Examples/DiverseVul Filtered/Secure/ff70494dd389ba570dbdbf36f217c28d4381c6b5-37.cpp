int getchancount(int valid)
{
    int i=0, chan_count=0;

    while(G.channels[i])
    {
        i++;
        if(G.channels[i] != -1)
            chan_count++;
    }

    if(valid) return chan_count;
    return i;
}