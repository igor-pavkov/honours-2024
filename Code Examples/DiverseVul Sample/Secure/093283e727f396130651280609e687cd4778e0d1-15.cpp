void abst_box_del(GF_Box *s)
{
	GF_AdobeBootstrapInfoBox *ptr = (GF_AdobeBootstrapInfoBox *)s;
	if (ptr == NULL) return;

	if (ptr->movie_identifier)
		gf_free(ptr->movie_identifier);
	if (ptr->drm_data)
		gf_free(ptr->drm_data);
	if (ptr->meta_data)
		gf_free(ptr->meta_data);

	while (gf_list_count(ptr->server_entry_table)) {
		gf_free(gf_list_get(ptr->server_entry_table, 0));
		gf_list_rem(ptr->server_entry_table, 0);
	}
	gf_list_del(ptr->server_entry_table);

	while (gf_list_count(ptr->quality_entry_table)) {
		gf_free(gf_list_get(ptr->quality_entry_table, 0));
		gf_list_rem(ptr->quality_entry_table, 0);
	}
	gf_list_del(ptr->quality_entry_table);


	while (gf_list_count(ptr->segment_run_table_entries)) {
		gf_isom_box_del((GF_Box *)gf_list_get(ptr->segment_run_table_entries, 0));
		gf_list_rem(ptr->segment_run_table_entries, 0);
	}
	gf_list_del(ptr->segment_run_table_entries);

	while (gf_list_count(ptr->fragment_run_table_entries)) {
		gf_isom_box_del((GF_Box *)gf_list_get(ptr->fragment_run_table_entries, 0));
		gf_list_rem(ptr->fragment_run_table_entries, 0);
	}
	gf_list_del(ptr->fragment_run_table_entries);

	gf_free(ptr);
}