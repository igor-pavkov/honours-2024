GF_Err gen_sample_entry_Read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e = gf_isom_base_sample_entry_read((GF_SampleEntryBox *)s, bs);
	if (e) return e;
	ISOM_DECREASE_SIZE(s, 8);
	return gf_isom_box_array_read(s, bs, gf_isom_box_add_default);
}