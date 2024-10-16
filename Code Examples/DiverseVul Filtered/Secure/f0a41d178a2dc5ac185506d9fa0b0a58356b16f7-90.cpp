void gf_isom_parse_trif_info(const u8 *data, u32 size, u32 *id, u32 *independent, Bool *full_picture, u32 *x, u32 *y, u32 *w, u32 *h)
{
	GF_BitStream *bs;
	bs = gf_bs_new(data, size, GF_BITSTREAM_READ);
	*id = gf_bs_read_u16(bs);
	if (! gf_bs_read_int(bs, 1)) {
		*independent=0;
		*full_picture=0;
		*x = *y = *w = *h = 0;
	} else {
		*independent = gf_bs_read_int(bs, 2);
		*full_picture = (Bool)gf_bs_read_int(bs, 1);
		/*filter_disabled*/ gf_bs_read_int(bs, 1);
		/*has_dependency_list*/ gf_bs_read_int(bs, 1);
		gf_bs_read_int(bs, 2);
		*x = *full_picture ? 0 : gf_bs_read_u16(bs);
		*y = *full_picture ? 0 : gf_bs_read_u16(bs);
		*w = gf_bs_read_u16(bs);
		*h = gf_bs_read_u16(bs);
	}
	gf_bs_del(bs);
}