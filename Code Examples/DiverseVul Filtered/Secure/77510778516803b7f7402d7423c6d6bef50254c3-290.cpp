GF_Err dmlp_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_TrueHDConfigBox *ptr = (GF_TrueHDConfigBox *)s;
	ISOM_DECREASE_SIZE(ptr, 10)
	ptr->format_info = gf_bs_read_u32(bs);
	ptr->peak_data_rate = gf_bs_read_int(bs, 15);
	gf_bs_read_int(bs, 1);
	gf_bs_read_u32(bs);
	return GF_OK;
}