GF_Box *stsh_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_ShadowSyncBox, GF_ISOM_BOX_TYPE_STSH);

	tmp->entries = gf_list_new();
	if (!tmp->entries) {
		gf_free(tmp);
		return NULL;
	}
	return (GF_Box *)tmp;
}