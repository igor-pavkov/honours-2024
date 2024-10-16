GF_Err asrt_box_read(GF_Box *s, GF_BitStream *bs)
{
	unsigned int i;
	GF_AdobeSegmentRunTableBox *ptr = (GF_AdobeSegmentRunTableBox *)s;

	ISOM_DECREASE_SIZE(ptr, 1)
	ptr->quality_entry_count = gf_bs_read_u8(bs);
	if (ptr->size < ptr->quality_entry_count)
		return GF_ISOM_INVALID_FILE;

	for (i=0; i<ptr->quality_entry_count; i++) {
		int j=0;
		u32 tmp_strsize=(u32)ptr->size;
		char *tmp_str = (char*) gf_malloc(tmp_strsize);
		if (!tmp_str) return GF_OUT_OF_MEM;
		while (tmp_strsize) {
			tmp_str[j] = gf_bs_read_u8(bs);
			tmp_strsize--;
			if (!tmp_str[j])
				break;
			j++;
		}
		ISOM_DECREASE_SIZE(ptr, j)
		gf_list_insert(ptr->quality_segment_url_modifiers, tmp_str, i);
	}

	ISOM_DECREASE_SIZE(ptr, 4)
	ptr->segment_run_entry_count = gf_bs_read_u32(bs);
	if (ptr->size < ptr->segment_run_entry_count*8)
		return GF_ISOM_INVALID_FILE;

	for (i=0; i<ptr->segment_run_entry_count; i++) {
		GF_AdobeSegmentRunEntry *sre = gf_malloc(sizeof(GF_AdobeSegmentRunEntry));
		if (!sre) return GF_OUT_OF_MEM;
		ISOM_DECREASE_SIZE(ptr, 8)
		sre->first_segment = gf_bs_read_u32(bs);
		sre->fragment_per_segment = gf_bs_read_u32(bs);
		gf_list_insert(ptr->segment_run_entry_table, sre, i);
	}

	return GF_OK;
}