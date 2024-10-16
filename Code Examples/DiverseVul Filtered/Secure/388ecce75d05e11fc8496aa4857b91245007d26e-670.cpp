GF_Box *elst_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_EditListBox, GF_ISOM_BOX_TYPE_ELST);
	tmp->entryList = gf_list_new();
	if (!tmp->entryList) {
		gf_free(tmp);
		return NULL;
	}
	return (GF_Box *)tmp;
}