static int handler_28(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	i64 tlen;
	de_ucstring *s = NULL;
	struct pict_point pt;

	pict_read_point(c->infile, data_pos, &pt, "txLoc");
	tlen = (i64)de_getbyte(data_pos+4);
	s = ucstring_create(c);
	dbuf_read_to_ucstring(c->infile, data_pos+5, tlen, s, 0, DE_ENCODING_MACROMAN);
	de_dbg(c, "text: \"%s\"", ucstring_getpsz(s));
	*bytes_used = 5+tlen;
	ucstring_destroy(s);
	return 1;
}