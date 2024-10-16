GF_Err prhd_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ProjectionHeaderBox *ptr = (GF_ProjectionHeaderBox *)s;
	ISOM_DECREASE_SIZE(ptr, 12)
	ptr->yaw = (s32) gf_bs_read_u32(bs);
	ptr->pitch = (s32) gf_bs_read_u32(bs);
	ptr->roll = (s32) gf_bs_read_u32(bs);
	return GF_OK;
}