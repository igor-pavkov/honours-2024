GF_Err afrt_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	unsigned int i;
	GF_AdobeFragmentRunTableBox *ptr = (GF_AdobeFragmentRunTableBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_u32(bs, ptr->timescale);
	gf_bs_write_u8(bs, ptr->quality_entry_count);
	for (i=0; i<ptr->quality_entry_count; i++) {
		char *str = (char*)gf_list_get(ptr->quality_segment_url_modifiers, i);
		gf_bs_write_data(bs, str, (u32)strlen(str) + 1);
	}

	gf_bs_write_u32(bs, ptr->fragment_run_entry_count);
	for (i=0; i<ptr->fragment_run_entry_count; i++) {
		GF_AdobeFragmentRunEntry *fre = (GF_AdobeFragmentRunEntry *)gf_list_get(ptr->fragment_run_entry_table, i);
		gf_bs_write_u32(bs, fre->first_fragment);
		gf_bs_write_u64(bs, fre->first_fragment_timestamp);
		gf_bs_write_u32(bs, fre->fragment_duration);
		if (!fre->fragment_duration)
			gf_bs_write_u8(bs, fre->discontinuity_indicator);
	}

	return GF_OK;
}