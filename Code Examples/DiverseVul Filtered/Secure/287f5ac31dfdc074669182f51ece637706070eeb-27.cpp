static int handler_0c00(deark *c, lctx *d, i64 opcode, i64 data_pos, i64 *bytes_used)
{
	i64 hdrver;
	double hres, vres;
	struct pict_rect srcrect;

	hdrver = de_getu16be(data_pos);
	d->is_extended_v2 = (hdrver==0xfffe);

	de_dbg(c, "extended v2: %s", d->is_extended_v2?"yes":"no");
	if(d->is_extended_v2) {
		hres = pict_read_fixed(c->infile, data_pos+4);
		vres = pict_read_fixed(c->infile, data_pos+8);
		de_dbg(c, "dpi: %.2f"DE_CHAR_TIMES"%.2f", hres, vres);
		pict_read_rect(c->infile, data_pos+12, &srcrect, "srcRect");
	}

	return 1;
}