GF_Err mehd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_MovieExtendsHeaderBox *ptr = (GF_MovieExtendsHeaderBox *)s;

	if (ptr->version==1) {
		ISOM_DECREASE_SIZE(ptr, 8);
		ptr->fragment_duration = gf_bs_read_u64(bs);
	} else {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->fragment_duration = (u64) gf_bs_read_u32(bs);
	}
	return GF_OK;
}