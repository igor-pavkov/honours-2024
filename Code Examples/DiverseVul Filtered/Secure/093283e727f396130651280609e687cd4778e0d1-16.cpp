GF_Err afra_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	unsigned int i;
	GF_AdobeFragRandomAccessBox *ptr = (GF_AdobeFragRandomAccessBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;

	gf_bs_write_int(bs, ptr->long_ids, 1);
	gf_bs_write_int(bs, ptr->long_offsets, 1);
	gf_bs_write_int(bs, ptr->global_entries, 1);
	gf_bs_write_int(bs, 0, 5);
	gf_bs_write_u32(bs, ptr->time_scale);

	gf_bs_write_u32(bs, ptr->entry_count);
	for (i=0; i<ptr->entry_count; i++) {
		GF_AfraEntry *ae = (GF_AfraEntry *)gf_list_get(ptr->local_access_entries, i);
		gf_bs_write_u64(bs, ae->time);
		if (ptr->long_offsets)
			gf_bs_write_u64(bs, ae->offset);
		else
			gf_bs_write_u32(bs, (u32)ae->offset);
	}

	if (ptr->global_entries) {
		gf_bs_write_u32(bs, ptr->global_entry_count);
		for (i=0; i<ptr->global_entry_count; i++) {
			GF_GlobalAfraEntry *gae = (GF_GlobalAfraEntry *)gf_list_get(ptr->global_access_entries, i);
			gf_bs_write_u64(bs, gae->time);
			if (ptr->long_ids) {
				gf_bs_write_u32(bs, gae->segment);
				gf_bs_write_u32(bs, gae->fragment);
			} else {
				gf_bs_write_u16(bs, (u16)gae->segment);
				gf_bs_write_u16(bs, (u16)gae->fragment);
			}
			if (ptr->long_offsets) {
				gf_bs_write_u64(bs, gae->afra_offset);
				gf_bs_write_u64(bs, gae->offset_from_afra);
			} else {
				gf_bs_write_u32(bs, (u32)gae->afra_offset);
				gf_bs_write_u32(bs, (u32)gae->offset_from_afra);
			}
		}
	}

	return GF_OK;
}