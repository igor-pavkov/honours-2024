int open_outfile(__G)           /* return 1 if fail */
    __GDEF
{
#ifdef DLL
    if (G.redirect_data)
        return (redirect_outfile(__G) == FALSE);
#endif
#ifdef QDOS
    QFilename(__G__ G.filename);
#endif
#if (defined(DOS_FLX_NLM_OS2_W32) || defined(ATH_BEO_THS_UNX))
#ifdef BORLAND_STAT_BUG
    /* Borland 5.0's stat() barfs if the filename has no extension and the
     * file doesn't exist. */
    if (access(G.filename, 0) == -1) {
        FILE *tmp = fopen(G.filename, "wb+");

        /* file doesn't exist, so create a dummy file to keep stat() from
         * failing (will be over-written anyway) */
        fputc('0', tmp);  /* just to have something in the file */
        fclose(tmp);
    }
#endif /* BORLAND_STAT_BUG */
#ifdef SYMLINKS
    if (SSTAT(G.filename, &G.statbuf) == 0 ||
        lstat(G.filename, &G.statbuf) == 0)
#else
    if (SSTAT(G.filename, &G.statbuf) == 0)
#endif /* ?SYMLINKS */
    {
        Trace((stderr, "open_outfile:  stat(%s) returns 0:  file exists\n",
          FnFilter1(G.filename)));
#ifdef UNIXBACKUP
        if (uO.B_flag) {    /* do backup */
            char *tname;
            z_stat tmpstat;
            int blen, flen, tlen;

            blen = strlen(BackupSuffix);
            flen = strlen(G.filename);
            tlen = flen + blen + 6;    /* includes space for 5 digits */
            if (tlen >= FILNAMSIZ) {   /* in case name is too long, truncate */
                tname = (char *)malloc(FILNAMSIZ);
                if (tname == NULL)
                    return 1;                 /* in case we run out of space */
                tlen = FILNAMSIZ - 1 - blen;
                strcpy(tname, G.filename);    /* make backup name */
                tname[tlen] = '\0';
                if (flen > tlen) flen = tlen;
                tlen = FILNAMSIZ;
            } else {
                tname = (char *)malloc(tlen);
                if (tname == NULL)
                    return 1;                 /* in case we run out of space */
                strcpy(tname, G.filename);    /* make backup name */
            }
            strcpy(tname+flen, BackupSuffix);

            if (IS_OVERWRT_ALL) {
                /* If there is a previous backup file, delete it,
                 * otherwise the following rename operation may fail.
                 */
                if (SSTAT(tname, &tmpstat) == 0)
                    unlink(tname);
            } else {
                /* Check if backupname exists, and, if it's true, try
                 * appending numbers of up to 5 digits (or the maximum
                 * "unsigned int" number on 16-bit systems) to the
                 * BackupSuffix, until an unused name is found.
                 */
                unsigned maxtail, i;
                char *numtail = tname + flen + blen;

                /* take account of the "unsigned" limit on 16-bit systems: */
                maxtail = ( ((~0) >= 99999L) ? 99999 : (~0) );
                switch (tlen - flen - blen - 1) {
                    case 4: maxtail = 9999; break;
                    case 3: maxtail = 999; break;
                    case 2: maxtail = 99; break;
                    case 1: maxtail = 9; break;
                    case 0: maxtail = 0; break;
                }
                /* while filename exists */
                for (i = 0; (i < maxtail) && (SSTAT(tname, &tmpstat) == 0);)
                    sprintf(numtail,"%u", ++i);
            }

            if (rename(G.filename, tname) != 0) {   /* move file */
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(CannotRenameOldFile),
                  FnFilter1(G.filename), strerror(errno)));
                free(tname);
                return 1;
            }
            Trace((stderr, "open_outfile:  %s now renamed into %s\n",
              FnFilter1(G.filename), FnFilter2(tname)));
            free(tname);
        } else
#endif /* UNIXBACKUP */
        {
#ifdef DOS_FLX_OS2_W32
            if (!(G.statbuf.st_mode & S_IWRITE)) {
                Trace((stderr,
                  "open_outfile:  existing file %s is read-only\n",
                  FnFilter1(G.filename)));
                chmod(G.filename, S_IREAD | S_IWRITE);
                Trace((stderr, "open_outfile:  %s now writable\n",
                  FnFilter1(G.filename)));
            }
#endif /* DOS_FLX_OS2_W32 */
#ifdef NLM
            /* Give the file read/write permission (non-POSIX shortcut) */
            chmod(G.filename, 0);
#endif /* NLM */
            if (unlink(G.filename) != 0) {
                Info(slide, 0x401, ((char *)slide,
                  LoadFarString(CannotDeleteOldFile),
                  FnFilter1(G.filename), strerror(errno)));
                return 1;
            }
            Trace((stderr, "open_outfile:  %s now deleted\n",
              FnFilter1(G.filename)));
        }
    }
#endif /* DOS_FLX_NLM_OS2_W32 || ATH_BEO_THS_UNX */
#ifdef RISCOS
    if (SWI_OS_File_7(G.filename,0xDEADDEAD,0xDEADDEAD,G.lrec.ucsize)!=NULL) {
        Info(slide, 1, ((char *)slide, LoadFarString(CannotCreateFile),
          FnFilter1(G.filename), strerror(errno)));
        return 1;
    }
#endif /* RISCOS */
#ifdef TOPS20
    char *tfilnam;

    if ((tfilnam = (char *)malloc(2*strlen(G.filename)+1)) == (char *)NULL)
        return 1;
    strcpy(tfilnam, G.filename);
    upper(tfilnam);
    enquote(tfilnam);
    if ((G.outfile = fopen(tfilnam, FOPW)) == (FILE *)NULL) {
        Info(slide, 1, ((char *)slide, LoadFarString(CannotCreateFile),
          tfilnam, strerror(errno)));
        free(tfilnam);
        return 1;
    }
    free(tfilnam);
#else /* !TOPS20 */
#ifdef MTS
    if (uO.aflag)
        G.outfile = zfopen(G.filename, FOPWT);
    else
        G.outfile = zfopen(G.filename, FOPW);
    if (G.outfile == (FILE *)NULL) {
        Info(slide, 1, ((char *)slide, LoadFarString(CannotCreateFile),
          FnFilter1(G.filename), strerror(errno)));
        return 1;
    }
#else /* !MTS */
#ifdef DEBUG
    Info(slide, 1, ((char *)slide,
      "open_outfile:  doing fopen(%s) for reading\n", FnFilter1(G.filename)));
    if ((G.outfile = zfopen(G.filename, FOPR)) == (FILE *)NULL)
        Info(slide, 1, ((char *)slide,
          "open_outfile:  fopen(%s) for reading failed:  does not exist\n",
          FnFilter1(G.filename)));
    else {
        Info(slide, 1, ((char *)slide,
          "open_outfile:  fopen(%s) for reading succeeded:  file exists\n",
          FnFilter1(G.filename)));
        fclose(G.outfile);
    }
#endif /* DEBUG */
#ifdef NOVELL_BUG_FAILSAFE
    if (G.dne && ((G.outfile = zfopen(G.filename, FOPR)) != (FILE *)NULL)) {
        Info(slide, 0x401, ((char *)slide, LoadFarString(NovellBug),
          FnFilter1(G.filename)));
        fclose(G.outfile);
        return 1;   /* with "./" fix in checkdir(), should never reach here */
    }
#endif /* NOVELL_BUG_FAILSAFE */
    Trace((stderr, "open_outfile:  doing fopen(%s) for writing\n",
      FnFilter1(G.filename)));
    {
#if defined(ATH_BE_UNX) || defined(AOS_VS) || defined(QDOS) || defined(TANDEM)
        mode_t umask_sav = umask(0077);
#endif
#if defined(SYMLINKS) || defined(QLZIP)
        /* These features require the ability to re-read extracted data from
           the output files. Output files are created with Read&Write access.
         */
        G.outfile = zfopen(G.filename, FOPWR);
#else
        G.outfile = zfopen(G.filename, FOPW);
#endif
#if defined(ATH_BE_UNX) || defined(AOS_VS) || defined(QDOS) || defined(TANDEM)
        umask(umask_sav);
#endif
    }
    if (G.outfile == (FILE *)NULL) {
        Info(slide, 0x401, ((char *)slide, LoadFarString(CannotCreateFile),
          FnFilter1(G.filename), strerror(errno)));
        return 1;
    }
    Trace((stderr, "open_outfile:  fopen(%s) for writing succeeded\n",
      FnFilter1(G.filename)));
#endif /* !MTS */
#endif /* !TOPS20 */

#ifdef USE_FWRITE
#ifdef DOS_NLM_OS2_W32
    /* 16-bit MSC: buffer size must be strictly LESS than 32K (WSIZE):  bogus */
    setbuf(G.outfile, (char *)NULL);   /* make output unbuffered */
#else /* !DOS_NLM_OS2_W32 */
#ifndef RISCOS
#ifdef _IOFBF  /* make output fully buffered (works just about like write()) */
    setvbuf(G.outfile, (char *)slide, _IOFBF, WSIZE);
#else
    setbuf(G.outfile, (char *)slide);
#endif
#endif /* !RISCOS */
#endif /* ?DOS_NLM_OS2_W32 */
#endif /* USE_FWRITE */
#ifdef OS2_W32
    /* preallocate the final file size to prevent file fragmentation */
    SetFileSize(G.outfile, G.lrec.ucsize);
#endif
    return 0;

} /* end function open_outfile() */