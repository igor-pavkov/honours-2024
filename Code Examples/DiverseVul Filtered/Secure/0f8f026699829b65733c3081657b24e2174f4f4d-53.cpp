static int getuserpass(struct protstream *in, struct buf *buf)
{
    int c;

    buf_reset(buf);
    for (;;) {
	c = prot_getc(in);
	if (c == EOF || c == '\r' || c == '\n') {
	    buf_cstring(buf); /* appends a '\0' */
	    return c;
	}
	buf_putc(buf, c);
	if (buf_len(buf) > MAX_NNTP_ARG) {
	    fatal("argument too long", EC_IOERR);
	}
    }
}