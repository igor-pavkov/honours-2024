GF_Err elst_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	u32 nb_entries;
	GF_EditListBox *ptr = (GF_EditListBox *)s;
	if (!ptr) return GF_BAD_PARAM;

	nb_entries = gf_list_count(ptr->entryList);
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, nb_entries);
	for (i = 0; i < nb_entries; i++ ) {
		GF_EdtsEntry *p = (GF_EdtsEntry*)gf_list_get(ptr->entryList, i);
		if (ptr->version == 1) {
			gf_bs_write_u64(bs, p->segmentDuration);
			gf_bs_write_u64(bs, p->mediaTime);
		} else {
			gf_bs_write_u32(bs, (u32) p->segmentDuration);
			gf_bs_write_u32(bs, (s32) p->mediaTime);
		}
		gf_bs_write_u16(bs, p->mediaRate);
		gf_bs_write_u16(bs, 0);
	}
	return GF_OK;
}