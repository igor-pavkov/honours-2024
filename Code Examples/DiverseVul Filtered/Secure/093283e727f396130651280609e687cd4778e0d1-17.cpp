GF_Box *asrt_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeSegmentRunTableBox, GF_ISOM_BOX_TYPE_ASRT);
	tmp->quality_segment_url_modifiers = gf_list_new();
	tmp->segment_run_entry_table = gf_list_new();
	return (GF_Box *)tmp;
}