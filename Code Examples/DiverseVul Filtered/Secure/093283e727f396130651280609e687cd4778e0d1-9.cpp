GF_Err asrt_box_size(GF_Box *s)
{
	int i;
	GF_AdobeSegmentRunTableBox *ptr = (GF_AdobeSegmentRunTableBox *)s;

	s->size += 5;

	for (i=0; i<ptr->quality_entry_count; i++)
		s->size += strlen(gf_list_get(ptr->quality_segment_url_modifiers, i)) + 1;

	s->size += ptr->segment_run_entry_count * sizeof(GF_AdobeSegmentRunEntry);

	return GF_OK;
}