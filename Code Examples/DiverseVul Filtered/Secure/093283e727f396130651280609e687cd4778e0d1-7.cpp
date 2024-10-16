GF_Box *afrt_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeFragmentRunTableBox, GF_ISOM_BOX_TYPE_AFRT);
	tmp->quality_segment_url_modifiers = gf_list_new();
	tmp->fragment_run_entry_table = gf_list_new();
	return (GF_Box *)tmp;
}