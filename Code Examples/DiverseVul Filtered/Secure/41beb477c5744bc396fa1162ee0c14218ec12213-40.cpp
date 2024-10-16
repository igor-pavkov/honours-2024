int UZ_EXP UzpMessagePrnt(pG, buf, size, flag)
    zvoid *pG;   /* globals struct:  always passed */
    uch *buf;    /* preformatted string to be printed */
    ulg size;    /* length of string (may include nulls) */
    int flag;    /* flag bits */
{
    /* IMPORTANT NOTE:
     *    The name of the first parameter of UzpMessagePrnt(), which passes
     *    the "Uz_Globs" address, >>> MUST <<< be identical to the string
     *    expansion of the __G__ macro in the REENTRANT case (see globals.h).
     *    This name identity is mandatory for the LoadFarString() macro
     *    (in the SMALL_MEM case) !!!
     */
    int error;
    uch *q=buf, *endbuf=buf+(unsigned)size;
#ifdef MORE
    uch *p=buf;
#if (defined(SCREENWIDTH) && defined(SCREENLWRAP))
    int islinefeed = FALSE;
#endif
#endif
    FILE *outfp;


/*---------------------------------------------------------------------------
    These tests are here to allow fine-tuning of UnZip's output messages,
    but none of them will do anything without setting the appropriate bit
    in the flag argument of every Info() statement which is to be turned
    *off*.  That is, all messages are currently turned on for all ports.
    To turn off *all* messages, use the UzpMessageNull() function instead
    of this one.
  ---------------------------------------------------------------------------*/

#if (defined(OS2) && defined(DLL))
    if (MSG_NO_DLL2(flag))  /* if OS/2 DLL bit is set, do NOT print this msg */
        return 0;
#endif
#ifdef WINDLL
    if (MSG_NO_WDLL(flag))
        return 0;
#endif
#ifdef WINDLL
    if (MSG_NO_WGUI(flag))
        return 0;
#endif
/*
#ifdef ACORN_GUI
    if (MSG_NO_AGUI(flag))
        return 0;
#endif
 */
#ifdef DLL                 /* don't display message if data is redirected */
    if (((Uz_Globs *)pG)->redirect_data &&
        !((Uz_Globs *)pG)->redirect_text)
        return 0;
#endif

    if (MSG_STDERR(flag) && !((Uz_Globs *)pG)->UzO.tflag)
        outfp = (FILE *)stderr;
    else
        outfp = (FILE *)stdout;

#ifdef QUERY_TRNEWLN
    /* some systems require termination of query prompts with '\n' to force
     * immediate display */
    if (MSG_MNEWLN(flag)) {   /* assumes writable buffer (e.g., slide[]) */
        *endbuf++ = '\n';     /*  with room for one more char at end of buf */
        ++size;               /*  (safe assumption:  only used for four */
    }                         /*  short queries in extract.c and fileio.c) */
#endif

    if (MSG_TNEWLN(flag)) {   /* again assumes writable buffer:  fragile... */
        if ((!size && !((Uz_Globs *)pG)->sol) ||
            (size && (endbuf[-1] != '\n')))
        {
            *endbuf++ = '\n';
            ++size;
        }
    }

#ifdef MORE
# ifdef SCREENSIZE
    /* room for --More-- and one line of overlap: */
#  if (defined(SCREENWIDTH) && defined(SCREENLWRAP))
    SCREENSIZE(&((Uz_Globs *)pG)->height, &((Uz_Globs *)pG)->width);
#  else
    SCREENSIZE(&((Uz_Globs *)pG)->height, (int *)NULL);
#  endif
    ((Uz_Globs *)pG)->height -= 2;
# else
    /* room for --More-- and one line of overlap: */
    ((Uz_Globs *)pG)->height = SCREENLINES - 2;
#  if (defined(SCREENWIDTH) && defined(SCREENLWRAP))
    ((Uz_Globs *)pG)->width = SCREENWIDTH;
#  endif
# endif
#endif /* MORE */

    if (MSG_LNEWLN(flag) && !((Uz_Globs *)pG)->sol) {
        /* not at start of line:  want newline */
#ifdef OS2DLL
        if (!((Uz_Globs *)pG)->redirect_text) {
#endif
            putc('\n', outfp);
            fflush(outfp);
#ifdef MORE
            if (((Uz_Globs *)pG)->M_flag)
            {
#if (defined(SCREENWIDTH) && defined(SCREENLWRAP))
                ((Uz_Globs *)pG)->chars = 0;
#endif
                ++((Uz_Globs *)pG)->numlines;
                ++((Uz_Globs *)pG)->lines;
                if (((Uz_Globs *)pG)->lines >= ((Uz_Globs *)pG)->height)
                    (*((Uz_Globs *)pG)->mpause)((zvoid *)pG,
                      LoadFarString(MorePrompt), 1);
            }
#endif /* MORE */
            if (MSG_STDERR(flag) && ((Uz_Globs *)pG)->UzO.tflag &&
                !isatty(1) && isatty(2))
            {
                /* error output from testing redirected:  also send to stderr */
                putc('\n', stderr);
                fflush(stderr);
            }
#ifdef OS2DLL
        } else
           REDIRECTC('\n');
#endif
        ((Uz_Globs *)pG)->sol = TRUE;
    }

    /* put zipfile name, filename and/or error/warning keywords here */

#ifdef MORE
    if (((Uz_Globs *)pG)->M_flag
#ifdef OS2DLL
         && !((Uz_Globs *)pG)->redirect_text
#endif
                                                 )
    {
        while (p < endbuf) {
            if (*p == '\n') {
#if (defined(SCREENWIDTH) && defined(SCREENLWRAP))
                islinefeed = TRUE;
            } else if (SCREENLWRAP) {
                if (*p == '\r') {
                    ((Uz_Globs *)pG)->chars = 0;
                } else {
#  ifdef TABSIZE
                    if (*p == '\t')
                        ((Uz_Globs *)pG)->chars +=
                            (TABSIZE - (((Uz_Globs *)pG)->chars % TABSIZE));
                    else
#  endif
                        ++((Uz_Globs *)pG)->chars;

                    if (((Uz_Globs *)pG)->chars >= ((Uz_Globs *)pG)->width)
                        islinefeed = TRUE;
                }
            }
            if (islinefeed) {
                islinefeed = FALSE;
                ((Uz_Globs *)pG)->chars = 0;
#endif /* (SCREENWIDTH && SCREEN_LWRAP) */
                ++((Uz_Globs *)pG)->numlines;
                ++((Uz_Globs *)pG)->lines;
                if (((Uz_Globs *)pG)->lines >= ((Uz_Globs *)pG)->height)
                {
                    if ((error = WriteTxtErr(q, p-q+1, outfp)) != 0)
                        return error;
                    fflush(outfp);
                    ((Uz_Globs *)pG)->sol = TRUE;
                    q = p + 1;
                    (*((Uz_Globs *)pG)->mpause)((zvoid *)pG,
                      LoadFarString(MorePrompt), 1);
                }
            }
            INCSTR(p);
        } /* end while */
        size = (ulg)(p - q);   /* remaining text */
    }
#endif /* MORE */

    if (size) {
#ifdef OS2DLL
        if (!((Uz_Globs *)pG)->redirect_text) {
#endif
            if ((error = WriteTxtErr(q, size, outfp)) != 0)
                return error;
#ifndef VMS     /* 2005-09-16 SMS.  See note at "WriteTxtErr()", above. */
            fflush(outfp);
#endif
            if (MSG_STDERR(flag) && ((Uz_Globs *)pG)->UzO.tflag &&
                !isatty(1) && isatty(2))
            {
                /* error output from testing redirected:  also send to stderr */
                if ((error = WriteTxtErr(q, size, stderr)) != 0)
                    return error;
                fflush(stderr);
            }
#ifdef OS2DLL
        } else {                /* GRR:  this is ugly:  hide with macro */
            if ((error = REDIRECTPRINT(q, size)) != 0)
                return error;
        }
#endif /* OS2DLL */
        ((Uz_Globs *)pG)->sol = (endbuf[-1] == '\n');
    }
    return 0;

} /* end function UzpMessagePrnt() */