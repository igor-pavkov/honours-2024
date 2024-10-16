static int handler_2b(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	i64 tlen;
	i64 dh, dv;
	de_ucstring *s = NULL;

	dh = (i64)de_getbyte(data_pos);
	dv = (i64)de_getbyte(data_pos+1);
	de_dbg(c, "dh,dv: (%d,%d)", (int)dh, (int)dv);

	tlen = (i64)de_getbyte(data_pos+2);
	de_dbg(c, "text size: %d", (int)tlen);
	*bytes_used = 3+tlen;

	s = ucstring_create(c);
	dbuf_read_to_ucstring(c->infile, data_pos+3, tlen, s, 0, DE_ENCODING_MACROMAN);
	de_dbg(c, "text: \"%s\"", ucstring_getpsz(s));

	return 1;
}