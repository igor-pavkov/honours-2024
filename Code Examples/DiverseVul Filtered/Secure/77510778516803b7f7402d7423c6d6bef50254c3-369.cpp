GF_Err co64_box_read(GF_Box *s,GF_BitStream *bs)
{
	u32 entries;
	GF_ChunkLargeOffsetBox *ptr = (GF_ChunkLargeOffsetBox *) s;
	ptr->nb_entries = gf_bs_read_u32(bs);

	ISOM_DECREASE_SIZE(ptr, 4)

	if ((u64)ptr->nb_entries > ptr->size / 8 || (u64)ptr->nb_entries > (u64)SIZE_MAX/sizeof(u64)) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in co64\n", ptr->nb_entries));
		return GF_ISOM_INVALID_FILE;
	}

	ptr->offsets = (u64 *) gf_malloc(ptr->nb_entries * sizeof(u64) );
	if (ptr->offsets == NULL) return GF_OUT_OF_MEM;
	ptr->alloc_size = ptr->nb_entries;
	for (entries = 0; entries < ptr->nb_entries; entries++) {
		ptr->offsets[entries] = gf_bs_read_u64(bs);
	}
	return GF_OK;
}