GF_Err gitn_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_Err e;
	GroupIdToNameBox *ptr = (GroupIdToNameBox *)s;

	ISOM_DECREASE_SIZE(ptr, 2);
	ptr->nb_entries = gf_bs_read_u16(bs);
	if (ptr->size / 4 < ptr->nb_entries)
		return GF_ISOM_INVALID_FILE;

	GF_SAFE_ALLOC_N(ptr->entries, ptr->nb_entries, GroupIdNameEntry);
	if (!ptr->entries) return GF_OUT_OF_MEM;

	for (i=0; i<ptr->nb_entries; i++) {
		ISOM_DECREASE_SIZE(ptr, 4);
		ptr->entries[i].group_id = gf_bs_read_u32(bs);

		e = gf_isom_read_null_terminated_string(s, bs, ptr->size, &ptr->entries[i].name);
		if (e) return e;
	}
	return GF_OK;
}