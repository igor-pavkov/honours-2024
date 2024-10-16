static int decode_bitmap(deark *c, lctx *d, struct fmtutil_macbitmap_info *bi, i64 pos)
{
	i64 j;
	dbuf *unc_pixels = NULL;
	de_bitmap *img = NULL;
	de_finfo *fi = NULL;
	i64 bytecount;
	i64 bitmapsize;
	int dst_nsamples;
	struct de_dfilter_in_params dcmpri;
	struct de_dfilter_out_params dcmpro;
	struct de_dfilter_results dres;

	de_dfilter_init_objects(c, &dcmpri, &dcmpro, &dres);

	bi->rowspan = bi->rowbytes;
	if(bi->pixelsize==32 && bi->cmpcount==3 && bi->cmpsize==8) {
		bi->rowspan = (bi->rowbytes/4)*3;
	}

	bitmapsize = bi->height * bi->rowspan;
	unc_pixels = dbuf_create_membuf(c, bitmapsize, 1);

	dcmpri.f = c->infile;
	dcmpro.f = unc_pixels;

	for(j=0; j<bi->height; j++) {
		if(bi->packing_type==1 || bi->rowbytes<8) {
			bytecount = bi->rowbytes;
		}
		else if(bi->rowbytes > 250) {
			bytecount = de_getu16be(pos);
			pos+=2;
		}
		else {
			bytecount = (i64)de_getbyte(pos);
			pos+=1;
		}

		if(bi->packing_type==1 || bi->rowbytes<8) {
			dbuf_copy(c->infile, pos, bytecount, unc_pixels);
		}
		else if(bi->packing_type==3 && bi->pixelsize==16) {
			struct de_packbits_params pbparams;

			de_zeromem(&pbparams, sizeof(struct de_packbits_params));
			pbparams.is_packbits16 = 1;
			dcmpri.pos = pos;
			dcmpri.len = bytecount;
			fmtutil_decompress_packbits_ex(c, &dcmpri, &dcmpro, &dres, &pbparams);
		}
		else {
			dcmpri.pos = pos;
			dcmpri.len = bytecount;
			fmtutil_decompress_packbits_ex(c, &dcmpri, &dcmpro, &dres, NULL);
		}

		// Make sure the data decompressed to the right number of bytes.
		if(unc_pixels->len != (j+1)*bi->rowspan) {
			dbuf_truncate(unc_pixels, (j+1)*bi->rowspan);
		}

		pos += bytecount;
	}

	dst_nsamples = 3;
	if(bi->uses_pal) {
		if(de_is_grayscale_palette(bi->pal, bi->num_pal_entries)) {
			dst_nsamples = 1;
		}
	}

	img = de_bitmap_create2(c, bi->npwidth, bi->pdwidth, bi->height, dst_nsamples);

	fi = de_finfo_create(c);

	if(bi->hdpi>=1.0 && bi->vdpi>=1.0) {
		fi->density.code = DE_DENSITY_DPI;
		fi->density.xdens = bi->hdpi;
		fi->density.ydens = bi->vdpi;
	}

	if(bi->uses_pal) {
		decode_bitmap_paletted(c, d, bi, unc_pixels, img, pos);
	}
	else {
		if(bi->pixelsize==16) {
			decode_bitmap_rgb16(c, d, bi, unc_pixels, img, pos);
		}
		else {
			decode_bitmap_rgb24(c, d, bi, unc_pixels, img, pos);
		}
	}

	de_bitmap_write_to_file_finfo(img, fi, 0);

	de_bitmap_destroy(img);
	de_finfo_destroy(c, fi);
	dbuf_close(unc_pixels);
	return 1;
}