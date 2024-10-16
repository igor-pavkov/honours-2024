Bool Media_IsSampleSyncShadow(GF_ShadowSyncBox *stsh, u32 sampleNumber)
{
	u32 i;
	GF_StshEntry *ent;
	if (!stsh) return 0;
	i=0;
	while ((ent = (GF_StshEntry*)gf_list_enum(stsh->entries, &i))) {
		if ((u32) ent->syncSampleNumber == sampleNumber) return 1;
		else if ((u32) ent->syncSampleNumber > sampleNumber) return 0;
	}
	return 0;
}