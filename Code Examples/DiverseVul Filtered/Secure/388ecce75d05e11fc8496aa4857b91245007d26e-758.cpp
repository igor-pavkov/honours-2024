GF_Box *gnra_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_GenericAudioSampleEntryBox, GF_ISOM_BOX_TYPE_GNRA);
	gf_isom_audio_sample_entry_init((GF_AudioSampleEntryBox*) tmp);
	return (GF_Box *)tmp;
}