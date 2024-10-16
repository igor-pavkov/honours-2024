GF_Err afrt_box_size(GF_Box *s)
{
	u32 i;
	GF_AdobeFragmentRunTableBox *ptr = (GF_AdobeFragmentRunTableBox *)s;

	s->size += 5;

	for (i=0; i<ptr->quality_entry_count; i++)
		s->size += strlen(gf_list_get(ptr->quality_segment_url_modifiers, i)) + 1;

	s->size += 4;

	for (i=0; i<ptr->fragment_run_entry_count; i++) {
		GF_AdobeFragmentRunEntry *fre = (GF_AdobeFragmentRunEntry *)gf_list_get(ptr->fragment_run_entry_table, i);
		if (fre->fragment_duration)
			s->size += 16;
		else
			s->size += 17;
	}

	return GF_OK;
}