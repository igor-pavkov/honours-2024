GF_Err asrt_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	unsigned int i;
	GF_AdobeSegmentRunTableBox *ptr = (GF_AdobeSegmentRunTableBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u8(bs, ptr->quality_entry_count);
	for (i=0; i<ptr->quality_entry_count; i++) {
		char *str = (char*)gf_list_get(ptr->quality_segment_url_modifiers, i);
		gf_bs_write_data(bs, str, (u32)strlen(str) + 1);
	}

	gf_bs_write_u32(bs, ptr->segment_run_entry_count);
	for (i=0; i<ptr->segment_run_entry_count; i++) {
		GF_AdobeSegmentRunEntry *sre = (GF_AdobeSegmentRunEntry *)gf_list_get(ptr->segment_run_entry_table, i);
		gf_bs_write_u32(bs, sre->first_segment);
		gf_bs_write_u32(bs, sre->fragment_per_segment);
	}

	return GF_OK;
}