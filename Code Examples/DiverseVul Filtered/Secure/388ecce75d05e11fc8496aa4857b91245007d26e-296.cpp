GF_Box *traf_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_TrackFragmentBox, GF_ISOM_BOX_TYPE_TRAF);
	tmp->TrackRuns = gf_list_new();
	return (GF_Box *)tmp;
}