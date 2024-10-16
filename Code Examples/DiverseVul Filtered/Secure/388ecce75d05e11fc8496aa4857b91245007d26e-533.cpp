GF_Err elst_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 entries;
	s32 tr;
	u32 nb_entries;
	GF_EditListBox *ptr = (GF_EditListBox *)s;

	ISOM_DECREASE_SIZE(ptr, 4);
	nb_entries = gf_bs_read_u32(bs);

	if (ptr->version == 1) {
		if (nb_entries > ptr->size / 20) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in ctts\n", nb_entries));
			return GF_ISOM_INVALID_FILE;
		}
	} else {
		if (nb_entries > ptr->size / 12) {
			GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in ctts\n", nb_entries));
			return GF_ISOM_INVALID_FILE;
		}
	}


	for (entries = 0; entries < nb_entries; entries++) {
		GF_EdtsEntry *p = (GF_EdtsEntry *) gf_malloc(sizeof(GF_EdtsEntry));
		if (!p) return GF_OUT_OF_MEM;
		if (ptr->version == 1) {
			ISOM_DECREASE_SIZE(ptr, 16);
			p->segmentDuration = gf_bs_read_u64(bs);
			p->mediaTime = (s64) gf_bs_read_u64(bs);
		} else {
			ISOM_DECREASE_SIZE(ptr, 8);
			p->segmentDuration = gf_bs_read_u32(bs);
			tr = gf_bs_read_u32(bs);
			p->mediaTime = (s64) tr;
		}
		ISOM_DECREASE_SIZE(ptr, 4);
		p->mediaRate = gf_bs_read_u16(bs);
		gf_bs_read_u16(bs);
		gf_list_add(ptr->entryList, p);
	}
	return GF_OK;
}