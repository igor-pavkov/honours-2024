int open_input_file(__G)    /* return 1 if open failed */
    __GDEF
{
    /*
     *  open the zipfile for reading and in BINARY mode to prevent cr/lf
     *  translation, which would corrupt the bitstreams
     */

#ifdef VMS
    G.zipfd = open(G.zipfn, O_RDONLY, 0, OPNZIP_RMS_ARGS);
#else /* !VMS */
#ifdef MACOS
    G.zipfd = open(G.zipfn, 0);
#else /* !MACOS */
#ifdef CMS_MVS
    G.zipfd = vmmvs_open_infile(__G);
#else /* !CMS_MVS */
#ifdef USE_STRM_INPUT
    G.zipfd = fopen(G.zipfn, FOPR);
#else /* !USE_STRM_INPUT */
    G.zipfd = open(G.zipfn, O_RDONLY | O_BINARY);
#endif /* ?USE_STRM_INPUT */
#endif /* ?CMS_MVS */
#endif /* ?MACOS */
#endif /* ?VMS */

#ifdef USE_STRM_INPUT
    if (G.zipfd == NULL)
#else
    /* if (G.zipfd < 0) */  /* no good for Windows CE port */
    if (G.zipfd == -1)
#endif
    {
        Info(slide, 0x401, ((char *)slide, LoadFarString(CannotOpenZipfile),
          G.zipfn, strerror(errno)));
        return 1;
    }
    return 0;

} /* end function open_input_file() */