static int handler_2c(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	i64 n;
	i64 tlen;
	i64 id;
	de_ucstring *s = NULL;

	n = de_getu16be(data_pos);
	*bytes_used = 2+n;
	id = de_getu16be(data_pos+2);
	de_dbg(c, "old font id: %d", (int)id);
	tlen = (i64)de_getbyte(data_pos+4);
	s = ucstring_create(c);
	dbuf_read_to_ucstring(c->infile, data_pos+5, tlen, s, 0, DE_ENCODING_MACROMAN);
	de_dbg(c, "font name: \"%s\"", ucstring_getpsz(s));
	ucstring_destroy(s);
	return 1;
}