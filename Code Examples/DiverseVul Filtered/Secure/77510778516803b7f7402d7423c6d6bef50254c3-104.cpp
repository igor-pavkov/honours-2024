GF_Box *trgr_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackGroupBox, GF_ISOM_BOX_TYPE_TRGR);
	tmp->groups = gf_list_new();
	if (!tmp->groups) {
		gf_free(tmp);
		return NULL;
	}
	return (GF_Box *)tmp;
}