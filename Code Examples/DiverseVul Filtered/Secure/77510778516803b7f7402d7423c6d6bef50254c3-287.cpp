GF_Err stsg_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_SubTrackSampleGroupBox *ptr = (GF_SubTrackSampleGroupBox *)s;
	ISOM_DECREASE_SIZE(s, 6);
	ptr->grouping_type = gf_bs_read_u32(bs);
	ptr->nb_groups = gf_bs_read_u16(bs);
	ISOM_DECREASE_SIZE(s, ptr->nb_groups*4);
	GF_SAFE_ALLOC_N(ptr->group_description_index, ptr->nb_groups, u32);
	if (!ptr->group_description_index) return GF_OUT_OF_MEM;
	for (i = 0; i < ptr->nb_groups; i++) {
		ptr->group_description_index[i] = gf_bs_read_u32(bs);
	}
	return GF_OK;
}