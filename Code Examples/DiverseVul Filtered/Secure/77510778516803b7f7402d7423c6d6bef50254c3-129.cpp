GF_Err stsg_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_SubTrackSampleGroupBox *ptr = (GF_SubTrackSampleGroupBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->grouping_type);
	gf_bs_write_u16(bs, ptr->nb_groups);
	for (i = 0; i < ptr->nb_groups; i++) {
		gf_bs_write_u32(bs, ptr->group_description_index[i]);
	}
	return GF_OK;
}