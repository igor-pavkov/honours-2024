static void rewrite_nalus_list(GF_List *nalus, GF_BitStream *bs, Bool rewrite_start_codes, u32 nal_unit_size_field)
{
	u32 i, count = gf_list_count(nalus);
	for (i=0; i<count; i++) {
		GF_NALUFFParam *sl = (GF_NALUFFParam*)gf_list_get(nalus, i);
		if (rewrite_start_codes) gf_bs_write_u32(bs, 1);
		else gf_bs_write_int(bs, sl->size, 8*nal_unit_size_field);
		gf_bs_write_data(bs, sl->data, sl->size);
	}
}