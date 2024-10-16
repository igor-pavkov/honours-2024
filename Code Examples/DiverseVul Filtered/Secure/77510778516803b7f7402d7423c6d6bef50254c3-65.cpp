GF_Err csgp_box_write(GF_Box *s, GF_BitStream *bs)
{
	u32 i;
	GF_Err e;
	GF_CompactSampleGroupBox *ptr = (GF_CompactSampleGroupBox*)s;
	u32 pattern_size = get_size_by_code( ((ptr->flags>>4) & 0x3) );
	u32 scount_size = get_size_by_code( ((ptr->flags>>2) & 0x3) );
	u32 index_size = get_size_by_code( (ptr->flags & 0x3) );

	e = gf_isom_box_write_header(s, bs);
	if (e) return e;

	gf_bs_write_u8(bs, ptr->version);
	gf_bs_write_int(bs, ptr->flags, 24);
	gf_bs_write_u32(bs, ptr->grouping_type);

	if (ptr->flags & (1<<6))
		gf_bs_write_u32(bs, ptr->grouping_type_parameter);

	gf_bs_write_u32(bs, ptr->pattern_count);

	for (i = 0; i<ptr->pattern_count; i++ ) {
		gf_bs_write_int(bs, ptr->patterns[i].length, pattern_size);
		gf_bs_write_int(bs, ptr->patterns[i].sample_count, scount_size);
	}

	for (i = 0; i<ptr->pattern_count; i++ ) {
		u32 j;
		for (j=0; j<ptr->patterns[i].length; j++) {
			u32 idx = ptr->patterns[i].sample_group_description_indices[j];
			if (idx > 0x10000) {
				idx -= 0x10000;
				gf_bs_write_int(bs, 1, 1);
				gf_bs_write_int(bs, idx, index_size-1);
			} else {
				gf_bs_write_int(bs, idx, index_size);
			}
		}
	}
	gf_bs_align(bs);
	return GF_OK;
}