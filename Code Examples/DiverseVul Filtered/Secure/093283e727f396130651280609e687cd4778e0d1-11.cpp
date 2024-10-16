void afrt_box_del(GF_Box *s)
{
	GF_AdobeFragmentRunTableBox *ptr = (GF_AdobeFragmentRunTableBox *)s;
	if (ptr == NULL) return;

	while (gf_list_count(ptr->quality_segment_url_modifiers)) {
		gf_free(gf_list_get(ptr->quality_segment_url_modifiers, 0));
		gf_list_rem(ptr->quality_segment_url_modifiers, 0);
	}
	gf_list_del(ptr->quality_segment_url_modifiers);

	while (gf_list_count(ptr->fragment_run_entry_table)) {
		gf_free(gf_list_get(ptr->fragment_run_entry_table, 0));
		gf_list_rem(ptr->fragment_run_entry_table, 0);
	}
	gf_list_del(ptr->fragment_run_entry_table);

	gf_free(ptr);
}