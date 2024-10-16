static void read_src_dst_mode(deark *c, lctx *d, struct fmtutil_macbitmap_info *bi, i64 pos)
{
	struct pict_rect tmprect;
	i64 n;

	de_dbg(c, "src/dst/mode part of bitmap header, at %d", (int)pos);
	de_dbg_indent(c, 1);

	pict_read_rect(c->infile, pos, &tmprect, "srcRect");
	pos += 8;
	pict_read_rect(c->infile, pos, &tmprect, "dstRect");
	pos += 8;

	n = de_getu16be(pos);
	de_dbg(c, "transfer mode: %d", (int)n);
	//pos += 2;
	de_dbg_indent(c, -1);
}