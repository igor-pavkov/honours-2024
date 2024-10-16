GF_Box *moof_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MovieFragmentBox, GF_ISOM_BOX_TYPE_MOOF);
	tmp->TrackList = gf_list_new();
	return (GF_Box *)tmp;
}