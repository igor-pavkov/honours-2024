GF_Err svhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_SphericalVideoInfoBox *ptr = (GF_SphericalVideoInfoBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->string)
		gf_bs_write_data(bs, ptr->string, (u32) strlen(ptr->string));
	gf_bs_write_u8(bs, 0);
	return GF_OK;
}