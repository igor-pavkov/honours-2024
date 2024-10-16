static void decode_bitmap_rgb16(deark *c, lctx *d, struct fmtutil_macbitmap_info *bi,
	dbuf *unc_pixels, de_bitmap *img, i64 pos)
{
	i64 i, j;
	u8 c0, c1; //, cg, cb;
	u32 clr;

	for(j=0; j<bi->height; j++) {
		for(i=0; i<bi->pdwidth; i++) {
			c0 = dbuf_getbyte(unc_pixels, j*bi->rowspan + i*2);
			c1 = dbuf_getbyte(unc_pixels, j*bi->rowspan + i*2+1);
			clr = ((u32)c0 << 8)|c1;
			clr = de_rgb555_to_888(clr);
			de_bitmap_setpixel_rgb(img, i, j, clr);
		}
	}
}