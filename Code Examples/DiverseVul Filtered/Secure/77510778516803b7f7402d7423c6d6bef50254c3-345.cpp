GF_Err fecr_box_read(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	FECReservoirBox *ptr = (FECReservoirBox *)s;

	ISOM_DECREASE_SIZE(ptr, (ptr->version ? 4 : 2) );
	ptr->nb_entries = gf_bs_read_int(bs, ptr->version ? 32 : 16);

	if (ptr->nb_entries > ptr->size / (ptr->version ? 8 : 6) || (u64)ptr->nb_entries > (u64)SIZE_MAX/sizeof(FECReservoirEntry) ) {
		GF_LOG(GF_LOG_ERROR, GF_LOG_CONTAINER, ("[iso file] Invalid number of entries %d in fecr\n", ptr->nb_entries));
		return GF_ISOM_INVALID_FILE;
	}

	ISOM_DECREASE_SIZE(ptr, ptr->nb_entries * (ptr->version ? 8 : 6) );
	GF_SAFE_ALLOC_N(ptr->entries, ptr->nb_entries, FECReservoirEntry);
	if (!ptr->entries) return GF_OUT_OF_MEM;

	for (i=0; i<ptr->nb_entries; i++) {
		ptr->entries[i].item_id = gf_bs_read_int(bs, ptr->version ? 32 : 16);
		ptr->entries[i].symbol_count = gf_bs_read_u32(bs);
	}
	return GF_OK;
}