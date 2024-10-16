GF_Err lsr1_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_LASeRSampleEntryBox *ptr = (GF_LASeRSampleEntryBox*)s;

	e = gf_isom_base_sample_entry_read((GF_SampleEntryBox *)ptr, bs);
	if (e) return e;

	ISOM_DECREASE_SIZE(ptr, 8);

	return gf_isom_box_array_read(s, bs);
}