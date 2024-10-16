static int partflush(__G__ rawbuf, size, unshrink)
    __GDEF
    uch *rawbuf;        /* cannot be ZCONST, gets passed to (*G.message)() */
    ulg size;
    int unshrink;
#endif /* USE_DEFLATE64 && __16BIT__ */
{
    register uch *p;
    register uch *q;
    uch *transbuf;
#if (defined(SMALL_MEM) || defined(MED_MEM) || defined(VMS_TEXT_CONV))
    ulg transbufsiz;
#endif
    /* static int didCRlast = FALSE;    moved to globals.h */


/*---------------------------------------------------------------------------
    Compute the CRC first; if testing or if disk is full, that's it.
  ---------------------------------------------------------------------------*/

    G.crc32val = crc32(G.crc32val, rawbuf, (extent)size);

#ifdef DLL
    if ((G.statreportcb != NULL) &&
        (*G.statreportcb)(__G__ UZ_ST_IN_PROGRESS, G.zipfn, G.filename, NULL))
        return IZ_CTRLC;        /* cancel operation by user request */
#endif

    if (uO.tflag || size == 0L)  /* testing or nothing to write:  all done */
        return PK_OK;

    if (G.disk_full)
        return PK_DISK;         /* disk already full:  ignore rest of file */

/*---------------------------------------------------------------------------
    Write the bytes rawbuf[0..size-1] to the output device, first converting
    end-of-lines and ASCII/EBCDIC as needed.  If SMALL_MEM or MED_MEM are NOT
    defined, outbuf is assumed to be at least as large as rawbuf and is not
    necessarily checked for overflow.
  ---------------------------------------------------------------------------*/

    if (!G.pInfo->textmode) {   /* write raw binary data */
        /* GRR:  note that for standard MS-DOS compilers, size argument to
         * fwrite() can never be more than 65534, so WriteError macro will
         * have to be rewritten if size can ever be that large.  For now,
         * never more than 32K.  Also note that write() returns an int, which
         * doesn't necessarily limit size to 32767 bytes if write() is used
         * on 16-bit systems but does make it more of a pain; however, because
         * at least MSC 5.1 has a lousy implementation of fwrite() (as does
         * DEC Ultrix cc), write() is used anyway.
         */
#ifdef DLL
        if (G.redirect_data) {
#ifdef NO_SLIDE_REDIR
            if (writeToMemory(__G__ rawbuf, (extent)size)) return PK_ERR;
#else
            writeToMemory(__G__ rawbuf, (extent)size);
#endif
        } else
#endif
        if (!uO.cflag && WriteError(rawbuf, size, G.outfile))
            return disk_error(__G);
        else if (uO.cflag && (*G.message)((zvoid *)&G, rawbuf, size, 0))
            return PK_OK;
    } else {   /* textmode:  aflag is true */
        if (unshrink) {
            /* rawbuf = outbuf */
            transbuf = G.outbuf2;
#if (defined(SMALL_MEM) || defined(MED_MEM) || defined(VMS_TEXT_CONV))
            transbufsiz = TRANSBUFSIZ;
#endif
        } else {
            /* rawbuf = slide */
            transbuf = G.outbuf;
#if (defined(SMALL_MEM) || defined(MED_MEM) || defined(VMS_TEXT_CONV))
            transbufsiz = OUTBUFSIZ;
            Trace((stderr, "\ntransbufsiz = OUTBUFSIZ = %u\n",
                   (unsigned)OUTBUFSIZ));
#endif
        }
        if (G.newfile) {
#ifdef VMS_TEXT_CONV
            if (G.pInfo->hostnum == VMS_ && G.extra_field &&
                is_vms_varlen_txt(__G__ G.extra_field,
                                  G.lrec.extra_field_length))
                G.VMS_line_state = 0;    /* 0: ready to read line length */
            else
                G.VMS_line_state = -1;   /* -1: don't treat as VMS text */
#endif
            G.didCRlast = FALSE;         /* no previous buffers written */
            G.newfile = FALSE;
        }

#ifdef VMS_TEXT_CONV
        if (G.VMS_line_state >= 0)
        {
            p = rawbuf;
            q = transbuf;
            while ((extent)(p-rawbuf) < (extent)size) {
                switch (G.VMS_line_state) {

                    /* 0: ready to read line length */
                    case 0:
                        G.VMS_line_length = 0;
                        if ((extent)(p-rawbuf) == (extent)size-1) {
                            /* last char */
                            G.VMS_line_length = (unsigned)(*p++);
                            G.VMS_line_state = 1;
                        } else {
                            G.VMS_line_length = makeword(p);
                            p += 2;
                            G.VMS_line_state = 2;
                        }
                        G.VMS_line_pad =
                               ((G.VMS_line_length & 1) != 0); /* odd */
                        break;

                    /* 1: read one byte of length, need second */
                    case 1:
                        G.VMS_line_length += ((unsigned)(*p++) << 8);
                        G.VMS_line_state = 2;
                        break;

                    /* 2: ready to read VMS_line_length chars */
                    case 2:
                        {
                            extent remaining = (extent)size+(rawbuf-p);
                            extent outroom;

                            if (G.VMS_line_length < remaining) {
                                remaining = G.VMS_line_length;
                                G.VMS_line_state = 3;
                            }

                            outroom = transbuf+(extent)transbufsiz-q;
                            if (remaining >= outroom) {
                                remaining -= outroom;
                                for (;outroom > 0; p++, outroom--)
                                    *q++ = native(*p);
#ifdef DLL
                                if (G.redirect_data) {
                                    if (writeToMemory(__G__ transbuf,
                                          (extent)(q-transbuf))) return PK_ERR;
                                } else
#endif
                                if (!uO.cflag && WriteError(transbuf,
                                    (extent)(q-transbuf), G.outfile))
                                    return disk_error(__G);
                                else if (uO.cflag && (*G.message)((zvoid *)&G,
                                         transbuf, (ulg)(q-transbuf), 0))
                                    return PK_OK;
                                q = transbuf;
                                /* fall through to normal case */
                            }
                            G.VMS_line_length -= remaining;
                            for (;remaining > 0; p++, remaining--)
                                *q++ = native(*p);
                        }
                        break;

                    /* 3: ready to PutNativeEOL */
                    case 3:
                        if (q > transbuf+(extent)transbufsiz-lenEOL) {
#ifdef DLL
                            if (G.redirect_data) {
                                if (writeToMemory(__G__ transbuf,
                                      (extent)(q-transbuf))) return PK_ERR;
                            } else
#endif
                            if (!uO.cflag &&
                                WriteError(transbuf, (extent)(q-transbuf),
                                  G.outfile))
                                return disk_error(__G);
                            else if (uO.cflag && (*G.message)((zvoid *)&G,
                                     transbuf, (ulg)(q-transbuf), 0))
                                return PK_OK;
                            q = transbuf;
                        }
                        PutNativeEOL
                        G.VMS_line_state = G.VMS_line_pad ? 4 : 0;
                        break;

                    /* 4: ready to read pad byte */
                    case 4:
                        ++p;
                        G.VMS_line_state = 0;
                        break;
                }
            } /* end while */

        } else
#endif /* VMS_TEXT_CONV */

    /*-----------------------------------------------------------------------
        Algorithm:  CR/LF => native; lone CR => native; lone LF => native.
        This routine is only for non-raw-VMS, non-raw-VM/CMS files (i.e.,
        stream-oriented files, not record-oriented).
      -----------------------------------------------------------------------*/

        /* else not VMS text */ {
            p = rawbuf;
            if (*p == LF && G.didCRlast)
                ++p;
            G.didCRlast = FALSE;
            for (q = transbuf;  (extent)(p-rawbuf) < (extent)size;  ++p) {
                if (*p == CR) {           /* lone CR or CR/LF: treat as EOL  */
                    PutNativeEOL
                    if ((extent)(p-rawbuf) == (extent)size-1)
                        /* last char in buffer */
                        G.didCRlast = TRUE;
                    else if (p[1] == LF)  /* get rid of accompanying LF */
                        ++p;
                } else if (*p == LF)      /* lone LF */
                    PutNativeEOL
                else
#ifndef DOS_FLX_OS2_W32
                if (*p != CTRLZ)          /* lose all ^Z's */
#endif
                    *q++ = native(*p);

#if (defined(SMALL_MEM) || defined(MED_MEM))
# if (lenEOL == 1)   /* don't check unshrink:  both buffers small but equal */
                if (!unshrink)
# endif
                    /* check for danger of buffer overflow and flush */
                    if (q > transbuf+(extent)transbufsiz-lenEOL) {
                        Trace((stderr,
                          "p - rawbuf = %u   q-transbuf = %u   size = %lu\n",
                          (unsigned)(p-rawbuf), (unsigned)(q-transbuf), size));
                        if (!uO.cflag && WriteError(transbuf,
                            (extent)(q-transbuf), G.outfile))
                            return disk_error(__G);
                        else if (uO.cflag && (*G.message)((zvoid *)&G,
                                 transbuf, (ulg)(q-transbuf), 0))
                            return PK_OK;
                        q = transbuf;
                        continue;
                    }
#endif /* SMALL_MEM || MED_MEM */
            }
        }

    /*-----------------------------------------------------------------------
        Done translating:  write whatever we've got to file (or screen).
      -----------------------------------------------------------------------*/

        Trace((stderr, "p - rawbuf = %u   q-transbuf = %u   size = %lu\n",
          (unsigned)(p-rawbuf), (unsigned)(q-transbuf), size));
        if (q > transbuf) {
#ifdef DLL
            if (G.redirect_data) {
                if (writeToMemory(__G__ transbuf, (extent)(q-transbuf)))
                    return PK_ERR;
            } else
#endif
            if (!uO.cflag && WriteError(transbuf, (extent)(q-transbuf),
                G.outfile))
                return disk_error(__G);
            else if (uO.cflag && (*G.message)((zvoid *)&G, transbuf,
                (ulg)(q-transbuf), 0))
                return PK_OK;
        }
    }

    return PK_OK;

} /* end function flush() [resp. partflush() for 16-bit Deflate64 support] */