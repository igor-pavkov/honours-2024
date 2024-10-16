int seek_zipf(__G__ abs_offset)
    __GDEF
    zoff_t abs_offset;
{
/*
 *  Seek to the block boundary of the block which includes abs_offset,
 *  then read block into input buffer and set pointers appropriately.
 *  If block is already in the buffer, just set the pointers.  This function
 *  is used by do_seekable (process.c), extract_or_test_entrylist (extract.c)
 *  and do_string (fileio.c).  Also, a slightly modified version is embedded
 *  within extract_or_test_entrylist (extract.c).  readbyte() and readbuf()
 *  (fileio.c) are compatible.  NOTE THAT abs_offset is intended to be the
 *  "proper offset" (i.e., if there were no extra bytes prepended);
 *  cur_zipfile_bufstart contains the corrected offset.
 *
 *  Since seek_zipf() is never used during decompression, it is safe to
 *  use the slide[] buffer for the error message.
 *
 * returns PK error codes:
 *  PK_BADERR if effective offset in zipfile is negative
 *  PK_EOF if seeking past end of zipfile
 *  PK_OK when seek was successful
 */
    zoff_t request = abs_offset + G.extra_bytes;
    zoff_t inbuf_offset = request % INBUFSIZ;
    zoff_t bufstart = request - inbuf_offset;

    if (request < 0) {
        Info(slide, 1, ((char *)slide, LoadFarStringSmall(SeekMsg),
             G.zipfn, LoadFarString(ReportMsg)));
        return(PK_BADERR);
    } else if (bufstart != G.cur_zipfile_bufstart) {
        Trace((stderr,
          "fpos_zip: abs_offset = %s, G.extra_bytes = %s\n",
          FmZofft(abs_offset, NULL, NULL),
          FmZofft(G.extra_bytes, NULL, NULL)));
#ifdef USE_STRM_INPUT
        zfseeko(G.zipfd, bufstart, SEEK_SET);
        G.cur_zipfile_bufstart = zftello(G.zipfd);
#else /* !USE_STRM_INPUT */
        G.cur_zipfile_bufstart = zlseek(G.zipfd, bufstart, SEEK_SET);
#endif /* ?USE_STRM_INPUT */
        Trace((stderr,
          "       request = %s, (abs+extra) = %s, inbuf_offset = %s\n",
          FmZofft(request, NULL, NULL),
          FmZofft((abs_offset+G.extra_bytes), NULL, NULL),
          FmZofft(inbuf_offset, NULL, NULL)));
        Trace((stderr, "       bufstart = %s, cur_zipfile_bufstart = %s\n",
          FmZofft(bufstart, NULL, NULL),
          FmZofft(G.cur_zipfile_bufstart, NULL, NULL)));
        if ((G.incnt = read(G.zipfd, (char *)G.inbuf, INBUFSIZ)) <= 0)
            return(PK_EOF);
        G.incnt -= (int)inbuf_offset;
        G.inptr = G.inbuf + (int)inbuf_offset;
    } else {
        G.incnt += (G.inptr-G.inbuf) - (int)inbuf_offset;
        G.inptr = G.inbuf + (int)inbuf_offset;
    }
    return(PK_OK);
} /* end function seek_zipf() */