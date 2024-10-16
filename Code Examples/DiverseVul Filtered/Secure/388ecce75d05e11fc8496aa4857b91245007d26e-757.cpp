void stsh_box_del(GF_Box *s)
{
	u32 i = 0;
	GF_StshEntry *ent;
	GF_ShadowSyncBox *ptr = (GF_ShadowSyncBox *)s;
	if (ptr == NULL) return;
	while ( (ent = (GF_StshEntry *)gf_list_enum(ptr->entries, &i)) ) {
		gf_free(ent);
	}
	gf_list_del(ptr->entries);
	gf_free(ptr);
}