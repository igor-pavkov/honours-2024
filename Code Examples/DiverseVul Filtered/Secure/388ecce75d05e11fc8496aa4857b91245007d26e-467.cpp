GF_Err gnra_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_GenericAudioSampleEntryBox *ptr = (GF_GenericAudioSampleEntryBox *)s;

	//carefull we are not writing the box type but the entry type so switch for write
	ptr->type = ptr->EntryType;
	e = gf_isom_box_write_header(s, bs);
	if (e) return e;
	ptr->type = GF_ISOM_BOX_TYPE_GNRA;

	gf_isom_audio_sample_entry_write((GF_AudioSampleEntryBox *)ptr, bs);
	if (ptr->data) {
		gf_bs_write_data(bs,  ptr->data, ptr->data_size);
	}
	return GF_OK;
}