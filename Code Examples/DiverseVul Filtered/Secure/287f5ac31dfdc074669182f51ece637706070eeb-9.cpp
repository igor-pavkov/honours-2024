static int handler_98_9a(deark *c, lctx *d, i64 opcode, i64 pos1, i64 *bytes_used)
{
	struct fmtutil_macbitmap_info *bi = NULL;
	i64 pixdata_size = 0;
	i64 colortable_size = 0;
	int retval = 0;
	i64 pos;

	bi = de_malloc(c, sizeof(struct fmtutil_macbitmap_info));
	pos = pos1;

	if(opcode==0x9a || opcode==0x9b) {
		fmtutil_macbitmap_read_baseaddr(c, c->infile, bi, pos);
		pos += 4;
	}

	fmtutil_macbitmap_read_rowbytes_and_bounds(c, c->infile, bi, pos);
	pos += 10;

	if(bi->pixmap_flag) {
		fmtutil_macbitmap_read_pixmap_only_fields(c, c->infile, bi, pos);
		pos += 36;
	}

	if((opcode==0x90 || opcode==0x91 || opcode==0x98 || opcode==0x99) && bi->pixmap_flag) {
		// Prepare to read the palette
		bi->uses_pal = 1;
		bi->has_colortable = 1;
	}
	else if((opcode==0x90 || opcode==0x91 || opcode==0x98 || opcode==0x99) && !bi->pixmap_flag) {
		// Settings implied by the lack of a PixMap header
		bi->pixelsize = 1;
		bi->cmpcount = 1;
		bi->cmpsize = 1;
		bi->uses_pal = 1;
		bi->num_pal_entries = 2;
		bi->pal[0] = DE_STOCKCOLOR_WHITE;
		bi->pal[1] = DE_STOCKCOLOR_BLACK;
	}
	else if((opcode==0x9a || opcode==0x9b) && !bi->pixmap_flag) {
		de_err(c, "DirectBitsRect image without PixMap flag is not supported");
		goto done;
	}

	if(bi->has_colortable) {
		if(!fmtutil_macbitmap_read_colortable(c, c->infile, bi, pos, &colortable_size)) goto done;
		pos += colortable_size;
	}

	read_src_dst_mode(c, d, bi, pos);
	pos += 18;

	if(opcode==0x91 || opcode==0x99 || opcode==0x9b) {
		i64 rgnsize;

		de_dbg(c, "region at %"I64_FMT, pos);
		de_dbg_indent(c, 1);
		rgnsize = de_getu16be(pos);
		de_dbg(c, "region size: %d", (int)rgnsize);
		de_dbg_indent(c, -1);
		if(rgnsize<2) goto done;
		pos += rgnsize;
		if(!c->padpix) {
			de_info(c, "Note: Ignoring clipping region. Output image might have "
				"extraneous pixels.");
		}
	}

	if(!get_pixdata_size(c, d, bi, pos, &pixdata_size)) {
		goto done;
	}
	decode_pixdata(c, d, bi, pos);
	pos += pixdata_size;

	*bytes_used = pos - pos1;

	retval = 1;

done:
	de_free(c, bi);
	return retval;
}