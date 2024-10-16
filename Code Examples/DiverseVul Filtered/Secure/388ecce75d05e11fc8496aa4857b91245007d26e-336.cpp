GF_Err mehd_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_MovieExtendsHeaderBox *ptr = (GF_MovieExtendsHeaderBox *)s;
	GF_Err e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->version == 1) {
		gf_bs_write_u64(bs, ptr->fragment_duration);
	} else {
		gf_bs_write_u32(bs, (u32) ptr->fragment_duration);
	}
	return GF_OK;
}