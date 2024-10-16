static int handler_11(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	i64 ver;

	*bytes_used = 1;
	ver = de_getbyte(data_pos);
	de_dbg(c, "version: %d", (int)ver);

	if(ver==2) {
		d->version = 2;
	}
	else if(ver!=1) {
		de_err(c, "Unsupported PICT version: %d", (int)ver);
		return 0;
	}
	return 1;
}