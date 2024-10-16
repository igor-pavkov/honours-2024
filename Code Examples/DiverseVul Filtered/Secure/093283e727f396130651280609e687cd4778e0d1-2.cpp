GF_Err abst_box_size(GF_Box *s)
{
	GF_Err e;
	u32 i;
	GF_AdobeBootstrapInfoBox *ptr = (GF_AdobeBootstrapInfoBox *)s;

	s->size += 25
	           + (ptr->movie_identifier ? (strlen(ptr->movie_identifier) + 1) : 1)
	           + 1;

	for (i=0; i<ptr->server_entry_count; i++)
		s->size += strlen(gf_list_get(ptr->server_entry_table, i)) + 1;

	s->size += 1;

	for (i=0; i<ptr->quality_entry_count; i++)
		s->size += strlen(gf_list_get(ptr->quality_entry_table, i)) + 1;

	s->size += (ptr->drm_data ? (strlen(ptr->drm_data) + 1) : 1)
	           + (ptr->meta_data ? (strlen(ptr->meta_data) + 1) : 1)
	           + 1;

	for (i=0; i<ptr->segment_run_table_count; i++) {
		GF_Box *box = (GF_Box *)gf_list_get(ptr->segment_run_table_entries, i);
		e = gf_isom_box_size(box);
		if (e) return e;
		s->size += box->size;
	}

	s->size += 1;
	for (i=0; i<ptr->fragment_run_table_count; i++) {
		GF_Box *box = (GF_Box *)gf_list_get(ptr->fragment_run_table_entries, i);
		e = gf_isom_box_size(box);
		if (e) return e;
		s->size += box->size;
	}
	return GF_OK;
}