static void decode_bitmap_rgb24(deark *c, lctx *d, struct fmtutil_macbitmap_info *bi,
	dbuf *unc_pixels, de_bitmap *img, i64 pos)
{
	i64 i, j;
	u8 cr, cg, cb;

	for(j=0; j<bi->height; j++) {
		for(i=0; i<bi->pdwidth; i++) {
			cr = dbuf_getbyte(unc_pixels, j*bi->rowspan + (bi->cmpcount-3+0)*bi->pdwidth + i);
			cg = dbuf_getbyte(unc_pixels, j*bi->rowspan + (bi->cmpcount-3+1)*bi->pdwidth + i);
			cb = dbuf_getbyte(unc_pixels, j*bi->rowspan + (bi->cmpcount-3+2)*bi->pdwidth + i);
			de_bitmap_setpixel_rgb(img, i, j, DE_MAKE_RGB(cr,cg,cb));
		}
	}
}