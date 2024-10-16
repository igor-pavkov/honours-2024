int readbyte(__G)   /* refill inbuf and return a byte if available, else EOF */
    __GDEF
{
    if (G.mem_mode)
        return EOF;
    if (G.csize <= 0) {
        G.csize--;             /* for tests done after exploding */
        G.incnt = 0;
        return EOF;
    }
    if (G.incnt <= 0) {
        if ((G.incnt = read(G.zipfd, (char *)G.inbuf, INBUFSIZ)) == 0) {
            return EOF;
        } else if (G.incnt < 0) {  /* "fail" (abort, retry, ...) returns this */
            /* another hack, but no real harm copying same thing twice */
            (*G.message)((zvoid *)&G,
              (uch *)LoadFarString(ReadError),
              (ulg)strlen(LoadFarString(ReadError)), 0x401);
            echon();
#ifdef WINDLL
            longjmp(dll_error_return, 1);
#else
            DESTROYGLOBALS();
            EXIT(PK_BADERR);    /* totally bailing; better than lock-up */
#endif
        }
        G.cur_zipfile_bufstart += INBUFSIZ; /* always starts on block bndry */
        G.inptr = G.inbuf;
        defer_leftover_input(__G);           /* decrements G.csize */
    }

#if CRYPT
    if (G.pInfo->encrypted) {
        uch *p;
        int n;

        /* This was previously set to decrypt one byte beyond G.csize, when
         * incnt reached that far.  GRR said, "but it's required:  why?"  This
         * was a bug in fillinbuf() -- was it also a bug here?
         */
        for (n = G.incnt, p = G.inptr;  n--;  p++)
            zdecode(*p);
    }
#endif /* CRYPT */

    --G.incnt;
    return *G.inptr++;

} /* end function readbyte() */