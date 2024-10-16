void handler(signal)   /* upon interrupt, turn on echo and exit cleanly */
    int signal;
{
    GETGLOBALS();

#if !(defined(SIGBUS) || defined(SIGSEGV))      /* add a newline if not at */
    (*G.message)((zvoid *)&G, slide, 0L, 0x41); /*  start of line (to stderr; */
#endif                                          /*  slide[] should be safe) */

    echon();

#ifdef SIGBUS
    if (signal == SIGBUS) {
        Info(slide, 0x421, ((char *)slide, LoadFarString(ZipfileCorrupt),
          "bus error"));
        DESTROYGLOBALS();
        EXIT(PK_BADERR);
    }
#endif /* SIGBUS */

#ifdef SIGILL
    if (signal == SIGILL) {
        Info(slide, 0x421, ((char *)slide, LoadFarString(ZipfileCorrupt),
          "illegal instruction"));
        DESTROYGLOBALS();
        EXIT(PK_BADERR);
    }
#endif /* SIGILL */

#ifdef SIGSEGV
    if (signal == SIGSEGV) {
        Info(slide, 0x421, ((char *)slide, LoadFarString(ZipfileCorrupt),
          "segmentation violation"));
        DESTROYGLOBALS();
        EXIT(PK_BADERR);
    }
#endif /* SIGSEGV */

    /* probably ctrl-C */
    DESTROYGLOBALS();
#if defined(AMIGA) && defined(__SASC)
    _abort();
#endif
    EXIT(IZ_CTRLC);       /* was EXIT(0), then EXIT(PK_ERR) */
}