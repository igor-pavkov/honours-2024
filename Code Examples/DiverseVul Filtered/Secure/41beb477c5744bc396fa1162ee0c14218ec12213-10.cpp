char *fLoadFarStringSmall(__GPRO__ const char Far *sz)
{
    (void)zfstrcpy(G.rgchSmallBuffer, sz);
    return G.rgchSmallBuffer;
}