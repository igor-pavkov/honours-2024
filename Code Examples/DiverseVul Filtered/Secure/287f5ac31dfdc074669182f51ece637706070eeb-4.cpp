static int handler_DxText(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	i64 tlen;
	i64 dx;
	de_ucstring *s = NULL;

	dx = (i64)de_getbyte(data_pos);
	de_dbg(c, "%s: %d", opcode==0x2a?"dv":"dh", (int)dx);

	tlen = (i64)de_getbyte(data_pos+1);
	*bytes_used = 2+tlen;

	s = ucstring_create(c);
	dbuf_read_to_ucstring(c->infile, data_pos+2, tlen, s, 0, DE_ENCODING_MACROMAN);
	de_dbg(c, "text: \"%s\"", ucstring_getpsz(s));

	ucstring_destroy(s);
	return 1;
}