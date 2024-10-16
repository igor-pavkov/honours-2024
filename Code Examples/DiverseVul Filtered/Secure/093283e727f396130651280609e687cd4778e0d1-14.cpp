void asrt_box_del(GF_Box *s)
{
	GF_AdobeSegmentRunTableBox *ptr = (GF_AdobeSegmentRunTableBox *)s;
	if (ptr == NULL) return;

	while (gf_list_count(ptr->quality_segment_url_modifiers)) {
		gf_free(gf_list_get(ptr->quality_segment_url_modifiers, 0));
		gf_list_rem(ptr->quality_segment_url_modifiers, 0);
	}
	gf_list_del(ptr->quality_segment_url_modifiers);

	while (gf_list_count(ptr->segment_run_entry_table)) {
		gf_free(gf_list_get(ptr->segment_run_entry_table, 0));
		gf_list_rem(ptr->segment_run_entry_table, 0);
	}
	gf_list_del(ptr->segment_run_entry_table);

	gf_free(ptr);
}