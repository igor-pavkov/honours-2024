GF_Box *afra_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeFragRandomAccessBox, GF_ISOM_BOX_TYPE_AFRA);
	tmp->local_access_entries = gf_list_new();
	tmp->global_access_entries = gf_list_new();
	return (GF_Box *)tmp;
}