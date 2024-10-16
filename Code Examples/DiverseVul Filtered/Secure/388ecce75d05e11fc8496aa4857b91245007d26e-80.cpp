GF_Box *moov_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MovieBox, GF_ISOM_BOX_TYPE_MOOV);
	tmp->trackList = gf_list_new();
	if (!tmp->trackList) {
		gf_free(tmp);
		return NULL;
	}
	return (GF_Box *)tmp;
}