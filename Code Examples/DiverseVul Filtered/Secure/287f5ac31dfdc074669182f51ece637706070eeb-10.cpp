static int handler_pixpat(deark *c, lctx *d, i64 opcode, i64 pos1, i64 *bytes_used)
{
	unsigned int pattype;
	i64 pos = pos1;
	int needmsg = 1;
	int retval = 0;
	i64 colortable_size = 0;
	i64 pixdata_size = 0;
	struct fmtutil_macbitmap_info *bi = NULL;

	pattype = (unsigned int)de_getu16be_p(&pos);
	de_dbg(c, "PatType: %u", pattype);
	pos += 8; // Pat1Data

	if(pattype==2) { // ditherPat(?)
		pos += 6; // RGB
		retval = 1;
		goto done;
	}

	bi = de_malloc(c, sizeof(struct fmtutil_macbitmap_info));

	fmtutil_macbitmap_read_rowbytes_and_bounds(c, c->infile, bi, pos);
	pos += 10;
	fmtutil_macbitmap_read_pixmap_only_fields(c, c->infile, bi, pos);
	pos += 36;

	bi->uses_pal = 1;
	if(!fmtutil_macbitmap_read_colortable(c, c->infile, bi, pos, &colortable_size)) goto done;
	pos += colortable_size;

	if(!get_pixdata_size(c, d, bi, pos, &pixdata_size)) {
		goto done;
	}
	// Note: We could extract the "PixMap" pattern easily enough here, by calling
	// decode_pixdata(). But if we do that, maybe we should also extract the
	// Pat1Data data above, as well as other opcodes like BkPat.
	pos += pixdata_size;
	retval = 1;

done:
	if(!retval && needmsg) {
		de_err(c, "Failed to parse PixPat data");
	}
	if(retval) {
		*bytes_used = pos - pos1;
	}
	de_free(c, bi);
	return retval;
}