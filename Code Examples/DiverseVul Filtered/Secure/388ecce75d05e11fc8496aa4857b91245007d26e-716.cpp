GF_Err stsh_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_StshEntry *ent;
	GF_ShadowSyncBox *ptr = (GF_ShadowSyncBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, gf_list_count(ptr->entries));
	i=0;
	while ((ent = (GF_StshEntry *)gf_list_enum(ptr->entries, &i))) {
		gf_bs_write_u32(bs, ent->shadowedSampleNumber);
		gf_bs_write_u32(bs, ent->syncSampleNumber);
	}
	return GF_OK;
}