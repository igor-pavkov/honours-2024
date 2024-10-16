GF_Err stsh_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 count, i;
	GF_ShadowSyncBox *ptr = (GF_ShadowSyncBox *)s;

	ISOM_DECREASE_SIZE(s, 4)
	count = gf_bs_read_u32(bs);
	if (ptr->size / 8 < count)
		return GF_ISOM_INVALID_FILE;

	for (i = 0; i < count; i++) {
		GF_StshEntry *ent = (GF_StshEntry *) gf_malloc(sizeof(GF_StshEntry));
		if (!ent) return GF_OUT_OF_MEM;
		ent->shadowedSampleNumber = gf_bs_read_u32(bs);
		ent->syncSampleNumber = gf_bs_read_u32(bs);
		e = gf_list_add(ptr->entries, ent);
		if (e) return e;
	}
	return GF_OK;
}