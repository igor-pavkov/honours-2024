static void decode_bitmap_paletted(deark *c, lctx *d, struct fmtutil_macbitmap_info *bi,
	dbuf *unc_pixels, de_bitmap *img, i64 pos)
{
	i64 i, j;
	u8 b;
	u32 clr;

	for(j=0; j<bi->height; j++) {
		for(i=0; i<bi->pdwidth; i++) {
			b = de_get_bits_symbol(unc_pixels, bi->pixelsize, j*bi->rowspan, i);
			clr = bi->pal[(unsigned int)b];
			de_bitmap_setpixel_rgb(img, i, j, clr);
		}
	}
}