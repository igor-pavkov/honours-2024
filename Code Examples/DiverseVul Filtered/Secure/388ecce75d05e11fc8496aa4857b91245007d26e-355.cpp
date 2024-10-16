GF_Err audio_sample_entry_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	gf_isom_audio_sample_entry_write((GF_AudioSampleEntryBox*)s, bs);
	return GF_OK;
}