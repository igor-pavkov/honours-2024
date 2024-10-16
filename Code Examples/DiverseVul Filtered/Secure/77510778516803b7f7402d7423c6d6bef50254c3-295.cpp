GF_Err sgpd_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 entry_count;
	GF_SampleGroupDescriptionBox *p = (GF_SampleGroupDescriptionBox *)s;

	ISOM_DECREASE_SIZE(p, 8);
	p->grouping_type = gf_bs_read_u32(bs);

	if (p->version>=1) {
		ISOM_DECREASE_SIZE(p, 4);
		p->default_length = gf_bs_read_u32(bs);
	}
	if (p->version>=2) {
		ISOM_DECREASE_SIZE(p, 4);
		p->default_description_index = gf_bs_read_u32(bs);
	}
	entry_count = gf_bs_read_u32(bs);

	if (entry_count>p->size)
		return GF_ISOM_INVALID_FILE;

	while (entry_count) {
		void *ptr;
		u32 parsed_bytes=0;
		u32 size = p->default_length;
		if ((p->version>=1) && !size) {
			size = gf_bs_read_u32(bs);
			ISOM_DECREASE_SIZE(p, 4);
		}
		ptr = sgpd_parse_entry(p->grouping_type, bs, (s32) p->size, size, &parsed_bytes);
		//don't return an error, just stop parsing so that we skip over the sgpd box
		if (!ptr) return GF_OK;
		gf_list_add(p->group_descriptions, ptr);

		ISOM_DECREASE_SIZE(p, parsed_bytes);
		entry_count--;
	}
	return GF_OK;
}