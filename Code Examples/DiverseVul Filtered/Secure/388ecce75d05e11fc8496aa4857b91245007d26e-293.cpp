GF_Err ctts_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	u32 i;
	GF_CompositionOffsetBox *ptr = (GF_CompositionOffsetBox *)s;

	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	gf_bs_write_u32(bs, ptr->nb_entries);
	for (i=0; i<ptr->nb_entries; i++ ) {
		gf_bs_write_u32(bs, ptr->entries[i].sampleCount);
		if (ptr->version) {
			gf_bs_write_int(bs, ptr->entries[i].decodingOffset, 32);
		} else {
			gf_bs_write_u32(bs, (u32) ptr->entries[i].decodingOffset);
		}
	}
	return GF_OK;
}