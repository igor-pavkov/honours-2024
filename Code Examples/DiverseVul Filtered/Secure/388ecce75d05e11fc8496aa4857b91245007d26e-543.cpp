GF_Err gnra_box_size(GF_Box *s)
{
	GF_GenericAudioSampleEntryBox *ptr = (GF_GenericAudioSampleEntryBox *)s;
	s->type = GF_ISOM_BOX_TYPE_GNRA;
	gf_isom_audio_sample_entry_size((GF_AudioSampleEntryBox *)s);
	ptr->size += ptr->data_size;
	return GF_OK;
}