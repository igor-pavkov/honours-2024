char *fLoadFarString(__GPRO__ const char Far *sz)
{
    (void)zfstrcpy(G.rgchBigBuffer, sz);
    return G.rgchBigBuffer;
}