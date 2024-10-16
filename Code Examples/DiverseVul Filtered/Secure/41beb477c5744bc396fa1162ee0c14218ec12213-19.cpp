char *fLoadFarStringSmall2(__GPRO__ const char Far *sz)
{
    (void)zfstrcpy(G.rgchSmallBuffer2, sz);
    return G.rgchSmallBuffer2;
}