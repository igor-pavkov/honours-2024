GF_Err stvi_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_StereoVideoBox *ptr = (GF_StereoVideoBox *)s;

	ISOM_DECREASE_SIZE(ptr, 12);
	gf_bs_read_int(bs, 30);
	ptr->single_view_allowed = gf_bs_read_int(bs, 2);
	ptr->stereo_scheme = gf_bs_read_u32(bs);
	ptr->sit_len = gf_bs_read_u32(bs);
	ISOM_DECREASE_SIZE(ptr, ptr->sit_len);

	ptr->stereo_indication_type = gf_malloc(sizeof(char)*ptr->sit_len);
	if (!ptr->stereo_indication_type) return GF_OUT_OF_MEM;

	gf_bs_read_data(bs, ptr->stereo_indication_type, ptr->sit_len);
	return GF_OK;
}