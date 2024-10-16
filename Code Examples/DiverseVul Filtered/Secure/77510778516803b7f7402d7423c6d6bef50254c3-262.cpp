GF_Err dmlp_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_TrueHDConfigBox *ptr = (GF_TrueHDConfigBox *)s;

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->format_info);
	gf_bs_write_int(bs, ptr->peak_data_rate, 15);
	gf_bs_write_int(bs, 0, 1);
	gf_bs_write_u32(bs, 0);
	return GF_OK;
}