GF_Err prhd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ProjectionHeaderBox *ptr = (GF_ProjectionHeaderBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->yaw);
	gf_bs_write_u32(bs, ptr->pitch);
	gf_bs_write_u32(bs, ptr->roll);
	return GF_OK;
}