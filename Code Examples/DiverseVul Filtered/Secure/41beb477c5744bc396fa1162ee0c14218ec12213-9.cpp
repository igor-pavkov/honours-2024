void defer_leftover_input(__G)
    __GDEF
{
    if ((zoff_t)G.incnt > G.csize) {
        /* (G.csize < MAXINT), we can safely cast it to int !! */
        if (G.csize < 0L)
            G.csize = 0L;
        G.inptr_leftover = G.inptr + (int)G.csize;
        G.incnt_leftover = G.incnt - (int)G.csize;
        G.incnt = (int)G.csize;
    } else
        G.incnt_leftover = 0;
    G.csize -= G.incnt;
} /* end function defer_leftover_input() */