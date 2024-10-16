int check_frequency(struct wif *wi[], int cards)
{
    int i, freq;
    for(i=0; i<cards; i++)
    {
        freq = wi_get_freq(wi[i]);
        if(freq < 0) continue;
        if(G.frequency[i] != freq)
        {
            memset(G.message, '\x00', sizeof(G.message));
            snprintf(G.message, sizeof(G.message), "][ fixed frequency %s: %d ", wi_get_ifname(wi[i]), freq);
            wi_set_freq(wi[i], G.frequency[i]);
        }
    }
    return 0;
}