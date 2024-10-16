static int get_pixdata_size(deark *c, lctx *d, struct fmtutil_macbitmap_info *bi,
	i64 pos1, i64 *pixdata_size)
{
	i64 pos;
	i64 j;
	i64 bytecount;
	int retval = 0;

	pos = pos1;
	de_dbg(c, "PixData at %d", (int)pos);
	de_dbg_indent(c, 1);

	if(bi->height<0 || bi->height>65535) {
		de_err(c, "Invalid bitmap height (%d)", (int)bi->height);
		goto done;
	}

	// Make sure rowbytes is sane. We use it to decide how much memory to allocate.
	// Note: I've seen valid bitmaps with as many as 284 extra bytes per row.
	if(bi->rowbytes > (bi->npwidth * bi->pixelsize)/8 + 1000) {
		de_err(c, "Bad rowBytes value (%d)", (int)bi->rowbytes);
		goto done;
	}

	if(bi->packing_type>=3 || (bi->packing_type==0 && bi->rowbytes>=8)) {
		for(j=0; j<bi->height; j++) {
			if(bi->rowbytes > 250) {
				bytecount = de_getu16be(pos);
				pos+=2;
			}
			else {
				bytecount = (i64)de_getbyte(pos);
				pos+=1;
			}
			pos += bytecount;
		}
	}
	else if(bi->packing_type==1 || (bi->packing_type==0 && bi->rowbytes<8)) {
		pos += bi->rowbytes * bi->height; // uncompressed
	}
	else {
		de_err(c, "Unsupported packing type: %d", (int)bi->packing_type);
		goto done;
	}

	*pixdata_size = pos - pos1;
	de_dbg(c, "PixData size: %d", (int)*pixdata_size);
	retval = 1;

done:
	de_dbg_indent(c, -1);
	return retval;
}