int do_string(__G__ length, option)   /* return PK-type error code */
    __GDEF
    unsigned int length;        /* without prototype, ush converted to this */
    int option;
{
    unsigned comment_bytes_left;
    unsigned int block_len;
    int error=PK_OK;
#ifdef AMIGA
    char tmp_fnote[2 * AMIGA_FILENOTELEN];   /* extra room for squozen chars */
#endif


/*---------------------------------------------------------------------------
    This function processes arbitrary-length (well, usually) strings.  Four
    major options are allowed:  SKIP, wherein the string is skipped (pretty
    logical, eh?); DISPLAY, wherein the string is printed to standard output
    after undergoing any necessary or unnecessary character conversions;
    DS_FN, wherein the string is put into the filename[] array after under-
    going appropriate conversions (including case-conversion, if that is
    indicated: see the global variable pInfo->lcflag); and EXTRA_FIELD,
    wherein the `string' is assumed to be an extra field and is copied to
    the (freshly malloced) buffer G.extra_field.  The third option should
    be OK since filename is dimensioned at 1025, but we check anyway.

    The string, by the way, is assumed to start at the current file-pointer
    position; its length is given by 'length'.  So start off by checking the
    length of the string:  if zero, we're already done.
  ---------------------------------------------------------------------------*/

    if (!length)
        return PK_COOL;

    switch (option) {

#if (defined(SFX) && defined(CHEAP_SFX_AUTORUN))
    /*
     * Special case: See if the comment begins with an autorun command line.
     * Save that and display (or skip) the remainder.
     */

    case CHECK_AUTORUN:
    case CHECK_AUTORUN_Q:
        comment_bytes_left = length;
        if (length >= 10)
        {
            block_len = readbuf(__G__ (char *)G.outbuf, 10);
            if (block_len == 0)
                return PK_EOF;
            comment_bytes_left -= block_len;
            G.outbuf[block_len] = '\0';
            if (!strcmp((char *)G.outbuf, "$AUTORUN$>")) {
                char *eol;
                length -= 10;
                block_len = readbuf(__G__ G.autorun_command,
                                    MIN(length, sizeof(G.autorun_command)-1));
                if (block_len == 0)
                    return PK_EOF;
                comment_bytes_left -= block_len;
                G.autorun_command[block_len] = '\0';
                A_TO_N(G.autorun_command);
                eol = strchr(G.autorun_command, '\n');
                if (!eol)
                    eol = G.autorun_command + strlen(G.autorun_command) - 1;
                length -= eol + 1 - G.autorun_command;
                while (eol >= G.autorun_command && isspace(*eol))
                    *eol-- = '\0';
#if (defined(WIN32) && !defined(_WIN32_WCE))
                /* Win9x console always uses OEM character coding, and
                   WinNT console is set to OEM charset by default, too */
                INTERN_TO_OEM(G.autorun_command, G.autorun_command);
#endif /* (WIN32 && !_WIN32_WCE) */
            }
        }
        if (option == CHECK_AUTORUN_Q)  /* don't display the remainder */
            length = 0;
        /* seek to beginning of remaining part of comment -- rewind if */
        /* displaying entire comment, or skip to end if discarding it  */
        seek_zipf(__G__ G.cur_zipfile_bufstart - G.extra_bytes +
                  (G.inptr - G.inbuf) + comment_bytes_left - length);
        if (!length)
            break;
        /*  FALL THROUGH...  */
#endif /* SFX && CHEAP_SFX_AUTORUN */

    /*
     * First normal case:  print string on standard output.  First set loop
     * variables, then loop through the comment in chunks of OUTBUFSIZ bytes,
     * converting formats and printing as we go.  The second half of the
     * loop conditional was added because the file might be truncated, in
     * which case comment_bytes_left will remain at some non-zero value for
     * all time.  outbuf and slide are used as scratch buffers because they
     * are available (we should be either before or in between any file pro-
     * cessing).
     */

    case DISPLAY:
    case DISPL_8:
        comment_bytes_left = length;
        block_len = OUTBUFSIZ;       /* for the while statement, first time */
        while (comment_bytes_left > 0 && block_len > 0) {
            register uch *p = G.outbuf;
            register uch *q = G.outbuf;

            if ((block_len = readbuf(__G__ (char *)G.outbuf,
                   MIN((unsigned)OUTBUFSIZ, comment_bytes_left))) == 0)
                return PK_EOF;
            comment_bytes_left -= block_len;

            /* this is why we allocated an extra byte for outbuf:  terminate
             *  with zero (ASCIIZ) */
            G.outbuf[block_len] = '\0';

            /* remove all ASCII carriage returns from comment before printing
             * (since used before A_TO_N(), check for CR instead of '\r')
             */
            while (*p) {
                while (*p == CR)
                    ++p;
                *q++ = *p++;
            }
            /* could check whether (p - outbuf) == block_len here */
            *q = '\0';

            if (option == DISPL_8) {
                /* translate the text coded in the entry's host-dependent
                   "extended ASCII" charset into the compiler's (system's)
                   internal text code page */
                Ext_ASCII_TO_Native((char *)G.outbuf, G.pInfo->hostnum,
                                    G.pInfo->hostver, G.pInfo->HasUxAtt,
                                    FALSE);
#ifdef WINDLL
                /* translate to ANSI (RTL internal codepage may be OEM) */
                INTERN_TO_ISO((char *)G.outbuf, (char *)G.outbuf);
#else /* !WINDLL */
#if (defined(WIN32) && !defined(_WIN32_WCE))
                /* Win9x console always uses OEM character coding, and
                   WinNT console is set to OEM charset by default, too */
                INTERN_TO_OEM((char *)G.outbuf, (char *)G.outbuf);
#endif /* (WIN32 && !_WIN32_WCE) */
#endif /* ?WINDLL */
            } else {
                A_TO_N(G.outbuf);   /* translate string to native */
            }

#ifdef WINDLL
            /* ran out of local mem -- had to cheat */
            win_fprintf((zvoid *)&G, stdout, (extent)(q-G.outbuf),
                        (char *)G.outbuf);
            win_fprintf((zvoid *)&G, stdout, 2, (char *)"\n\n");
#else /* !WINDLL */
#ifdef NOANSIFILT       /* GRR:  can ANSI be used with EBCDIC? */
            (*G.message)((zvoid *)&G, G.outbuf, (ulg)(q-G.outbuf), 0);
#else /* ASCII, filter out ANSI escape sequences and handle ^S (pause) */
            p = G.outbuf - 1;
            q = slide;
            while (*++p) {
                int pause = FALSE;

                if (*p == 0x1B) {          /* ASCII escape char */
                    *q++ = '^';
                    *q++ = '[';
                } else if (*p == 0x13) {   /* ASCII ^S (pause) */
                    pause = TRUE;
                    if (p[1] == LF)        /* ASCII LF */
                        *q++ = *++p;
                    else if (p[1] == CR && p[2] == LF) {  /* ASCII CR LF */
                        *q++ = *++p;
                        *q++ = *++p;
                    }
                } else
                    *q++ = *p;
                if ((unsigned)(q-slide) > WSIZE-3 || pause) {   /* flush */
                    (*G.message)((zvoid *)&G, slide, (ulg)(q-slide), 0);
                    q = slide;
                    if (pause && G.extract_flag) /* don't pause for list/test */
                        (*G.mpause)((zvoid *)&G, LoadFarString(QuitPrompt), 0);
                }
            }
            (*G.message)((zvoid *)&G, slide, (ulg)(q-slide), 0);
#endif /* ?NOANSIFILT */
#endif /* ?WINDLL */
        }
        /* add '\n' if not at start of line */
        (*G.message)((zvoid *)&G, slide, 0L, 0x40);
        break;

    /*
     * Second case:  read string into filename[] array.  The filename should
     * never ever be longer than FILNAMSIZ-1 (1024), but for now we'll check,
     * just to be sure.
     */

    case DS_FN:
    case DS_FN_L:
#ifdef UNICODE_SUPPORT
        /* get the whole filename as need it for Unicode checksum */
        if (G.fnfull_bufsize <= length) {
            extent fnbufsiz = FILNAMSIZ;

            if (fnbufsiz <= length)
                fnbufsiz = length + 1;
            if (G.filename_full)
                free(G.filename_full);
            G.filename_full = malloc(fnbufsiz);
            if (G.filename_full == NULL)
                return PK_MEM;
            G.fnfull_bufsize = fnbufsiz;
        }
        if (readbuf(__G__ G.filename_full, length) == 0)
            return PK_EOF;
        G.filename_full[length] = '\0';      /* terminate w/zero:  ASCIIZ */

        /* if needed, chop off end so standard filename is a valid length */
        if (length >= FILNAMSIZ) {
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(FilenameTooLongTrunc)));
            error = PK_WARN;
            length = FILNAMSIZ - 1;
        }
        /* no excess size */
        block_len = 0;
        strncpy(G.filename, G.filename_full, length);
        G.filename[length] = '\0';      /* terminate w/zero:  ASCIIZ */
#else /* !UNICODE_SUPPORT */
        if (length >= FILNAMSIZ) {
            Info(slide, 0x401, ((char *)slide,
              LoadFarString(FilenameTooLongTrunc)));
            error = PK_WARN;
            /* remember excess length in block_len */
            block_len = length - (FILNAMSIZ - 1);
            length = FILNAMSIZ - 1;
        } else
            /* no excess size */
            block_len = 0;
        if (readbuf(__G__ G.filename, length) == 0)
            return PK_EOF;
        G.filename[length] = '\0';      /* terminate w/zero:  ASCIIZ */
#endif /* ?UNICODE_SUPPORT */

        /* translate the Zip entry filename coded in host-dependent "extended
           ASCII" into the compiler's (system's) internal text code page */
        Ext_ASCII_TO_Native(G.filename, G.pInfo->hostnum, G.pInfo->hostver,
                            G.pInfo->HasUxAtt, (option == DS_FN_L));

        if (G.pInfo->lcflag)      /* replace with lowercase filename */
            STRLOWER(G.filename, G.filename);

        if (G.pInfo->vollabel && length > 8 && G.filename[8] == '.') {
            char *p = G.filename+8;
            while (*p++)
                p[-1] = *p;  /* disk label, and 8th char is dot:  remove dot */
        }

        if (!block_len)         /* no overflow, we're done here */
            break;

        /*
         * We truncated the filename, so print what's left and then fall
         * through to the SKIP routine.
         */
        Info(slide, 0x401, ((char *)slide, "[ %s ]\n", FnFilter1(G.filename)));
        length = block_len;     /* SKIP the excess bytes... */
        /*  FALL THROUGH...  */

    /*
     * Third case:  skip string, adjusting readbuf's internal variables
     * as necessary (and possibly skipping to and reading a new block of
     * data).
     */

    case SKIP:
        /* cur_zipfile_bufstart already takes account of extra_bytes, so don't
         * correct for it twice: */
        seek_zipf(__G__ G.cur_zipfile_bufstart - G.extra_bytes +
                  (G.inptr-G.inbuf) + length);
        break;

    /*
     * Fourth case:  assume we're at the start of an "extra field"; malloc
     * storage for it and read data into the allocated space.
     */

    case EXTRA_FIELD:
        if (G.extra_field != (uch *)NULL)
            free(G.extra_field);
        if ((G.extra_field = (uch *)malloc(length)) == (uch *)NULL) {
            Info(slide, 0x401, ((char *)slide, LoadFarString(ExtraFieldTooLong),
              length));
            /* cur_zipfile_bufstart already takes account of extra_bytes,
             * so don't correct for it twice: */
            seek_zipf(__G__ G.cur_zipfile_bufstart - G.extra_bytes +
                      (G.inptr-G.inbuf) + length);
        } else {
            if (readbuf(__G__ (char *)G.extra_field, length) == 0)
                return PK_EOF;
            /* Looks like here is where extra fields are read */
            getZip64Data(__G__ G.extra_field, length);
#ifdef UNICODE_SUPPORT
            G.unipath_filename = NULL;
            if (G.UzO.U_flag < 2) {
              /* check if GPB11 (General Purpuse Bit 11) is set indicating
                 the standard path and comment are UTF-8 */
              if (G.pInfo->GPFIsUTF8) {
                /* if GPB11 set then filename_full is untruncated UTF-8 */
                G.unipath_filename = G.filename_full;
              } else {
                /* Get the Unicode fields if exist */
                getUnicodeData(__G__ G.extra_field, length);
                if (G.unipath_filename && strlen(G.unipath_filename) == 0) {
                  /* the standard filename field is UTF-8 */
                  free(G.unipath_filename);
                  G.unipath_filename = G.filename_full;
                }
              }
              if (G.unipath_filename) {
# ifdef UTF8_MAYBE_NATIVE
                if (G.native_is_utf8
#  ifdef UNICODE_WCHAR
                    && (!G.unicode_escape_all)
#  endif
                   ) {
                  strncpy(G.filename, G.unipath_filename, FILNAMSIZ - 1);
                  /* make sure filename is short enough */
                  if (strlen(G.unipath_filename) >= FILNAMSIZ) {
                    G.filename[FILNAMSIZ - 1] = '\0';
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(UFilenameTooLongTrunc)));
                    error = PK_WARN;
                  }
                }
#  ifdef UNICODE_WCHAR
                else
#  endif
# endif /* UTF8_MAYBE_NATIVE */
# ifdef UNICODE_WCHAR
                {
                  char *fn;

                  /* convert UTF-8 to local character set */
                  fn = utf8_to_local_string(G.unipath_filename,
                                            G.unicode_escape_all);
                  /* make sure filename is short enough */
                  if (strlen(fn) >= FILNAMSIZ) {
                    fn[FILNAMSIZ - 1] = '\0';
                    Info(slide, 0x401, ((char *)slide,
                      LoadFarString(UFilenameTooLongTrunc)));
                    error = PK_WARN;
                  }
                  /* replace filename with converted UTF-8 */
                  strcpy(G.filename, fn);
                  free(fn);
                }
# endif /* UNICODE_WCHAR */
                if (G.unipath_filename != G.filename_full)
                  free(G.unipath_filename);
                G.unipath_filename = NULL;
              }
            }
#endif /* UNICODE_SUPPORT */
        }
        break;

#ifdef AMIGA
    /*
     * Fifth case, for the Amiga only:  take the comment that would ordinarily
     * be skipped over, and turn it into a 79 character string that will be
     * attached to the file as a "filenote" after it is extracted.
     */

    case FILENOTE:
        if ((block_len = readbuf(__G__ tmp_fnote, (unsigned)
                                 MIN(length, 2 * AMIGA_FILENOTELEN - 1))) == 0)
            return PK_EOF;
        if ((length -= block_len) > 0)  /* treat remainder as in case SKIP: */
            seek_zipf(__G__ G.cur_zipfile_bufstart - G.extra_bytes
                      + (G.inptr - G.inbuf) + length);
        /* convert multi-line text into single line with no ctl-chars: */
        tmp_fnote[block_len] = '\0';
        while ((short int) --block_len >= 0)
            if ((unsigned) tmp_fnote[block_len] < ' ')
                if (tmp_fnote[block_len+1] == ' ')     /* no excess */
                    strcpy(tmp_fnote+block_len, tmp_fnote+block_len+1);
                else
                    tmp_fnote[block_len] = ' ';
        tmp_fnote[AMIGA_FILENOTELEN - 1] = '\0';
        if (G.filenotes[G.filenote_slot])
            free(G.filenotes[G.filenote_slot]);     /* should not happen */
        G.filenotes[G.filenote_slot] = NULL;
        if (tmp_fnote[0]) {
            if (!(G.filenotes[G.filenote_slot] = malloc(strlen(tmp_fnote)+1)))
                return PK_MEM;
            strcpy(G.filenotes[G.filenote_slot], tmp_fnote);
        }
        break;
#endif /* AMIGA */

    } /* end switch (option) */

    return error;

} /* end function do_string() */