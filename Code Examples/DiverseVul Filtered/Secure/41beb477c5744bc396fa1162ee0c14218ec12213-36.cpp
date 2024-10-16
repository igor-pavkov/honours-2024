unsigned readbuf(__G__ buf, size)   /* return number of bytes read into buf */
    __GDEF
    char *buf;
    register unsigned size;
{
    register unsigned count;
    unsigned n;

    n = size;
    while (size) {
        if (G.incnt <= 0) {
            if ((G.incnt = read(G.zipfd, (char *)G.inbuf, INBUFSIZ)) == 0)
                return (n-size);
            else if (G.incnt < 0) {
                /* another hack, but no real harm copying same thing twice */
                (*G.message)((zvoid *)&G,
                  (uch *)LoadFarString(ReadError),  /* CANNOT use slide */
                  (ulg)strlen(LoadFarString(ReadError)), 0x401);
                return 0;  /* discarding some data; better than lock-up */
            }
            /* buffer ALWAYS starts on a block boundary:  */
            G.cur_zipfile_bufstart += INBUFSIZ;
            G.inptr = G.inbuf;
        }
        count = MIN(size, (unsigned)G.incnt);
        memcpy(buf, G.inptr, count);
        buf += count;
        G.inptr += count;
        G.incnt -= count;
        size -= count;
    }
    return n;

} /* end function readbuf() */