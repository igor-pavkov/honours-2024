GF_Box *mvex_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MovieExtendsBox, GF_ISOM_BOX_TYPE_MVEX);
	tmp->TrackExList = gf_list_new();
	if (!tmp->TrackExList) {
		gf_free(tmp);
		return NULL;
	}
	tmp->TrackExPropList = gf_list_new();
	if (!tmp->TrackExPropList) {
		gf_list_del(tmp->TrackExList);
		gf_free(tmp);
		return NULL;
	}
	return (GF_Box *)tmp;
}