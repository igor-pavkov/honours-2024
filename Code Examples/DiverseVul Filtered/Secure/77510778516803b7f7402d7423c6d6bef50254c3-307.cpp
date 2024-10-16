GF_Err trik_box_read(GF_Box *s,GF_BitStream *bs)
{
	u32 i;
	GF_TrickPlayBox *ptr = (GF_TrickPlayBox *) s;
	ptr->entry_count = (u32) ptr->size;
	if ((u64)ptr->entry_count > (u64)SIZE_MAX/sizeof(GF_TrickPlayBoxEntry)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid size %llu in trik\n", ptr->size));
		return GF_ISOM_INVALID_FILE;
	}
	ptr->entries = (GF_TrickPlayBoxEntry *) gf_malloc(ptr->entry_count * sizeof(GF_TrickPlayBoxEntry) );
	if (!ptr->entries) return GF_OUT_OF_MEM;

	for (i=0; i< ptr->entry_count; i++) {
		ptr->entries[i].pic_type = gf_bs_read_int(bs, 2);
		ptr->entries[i].dependency_level = gf_bs_read_int(bs, 6);
	}
	return GF_OK;
}