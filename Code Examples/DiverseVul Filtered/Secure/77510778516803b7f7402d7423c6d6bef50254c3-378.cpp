GF_Err txtc_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_TextConfigBox *ptr = (GF_TextConfigBox *)s;
	GF_Err e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	if (ptr->config)
		gf_bs_write_data(bs, ptr->config, (u32) strlen(ptr->config));
	gf_bs_write_u8(bs, 0);
	return GF_OK;
}