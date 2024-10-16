GF_Err sbgp_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_SampleGroupBox *ptr = (GF_SampleGroupBox *)s;

	ISOM_DECREASE_SIZE(ptr, 8);
	ptr->grouping_type = gf_bs_read_u32(bs);

	if (ptr->version==1) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->grouping_type_parameter = gf_bs_read_u32(bs);
	}
	ptr->entry_count = gf_bs_read_u32(bs);

	if (ptr->size < sizeof(GF_SampleGroupEntry)*ptr->entry_count || (u64)ptr->entry_count > (u64)SIZE_MAX/sizeof(GF_SampleGroupEntry))
	    return GF_ISOM_INVALID_FILE;

	ptr->sample_entries = gf_malloc(sizeof(GF_SampleGroupEntry)*ptr->entry_count);
	if (!ptr->sample_entries) return GF_OUT_OF_MEM;

	for (i=0; i<ptr->entry_count; i++) {
		ISOM_DECREASE_SIZE(ptr, 8);
		ptr->sample_entries[i].sample_count = gf_bs_read_u32(bs);
		ptr->sample_entries[i].group_description_index = gf_bs_read_u32(bs);
	}
	return GF_OK;
}