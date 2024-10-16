GF_Box *abst_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_AdobeBootstrapInfoBox, GF_ISOM_BOX_TYPE_ABST);
	tmp->server_entry_table = gf_list_new();
	tmp->quality_entry_table = gf_list_new();
	tmp->segment_run_table_entries = gf_list_new();
	tmp->fragment_run_table_entries = gf_list_new();
	return (GF_Box *)tmp;
}