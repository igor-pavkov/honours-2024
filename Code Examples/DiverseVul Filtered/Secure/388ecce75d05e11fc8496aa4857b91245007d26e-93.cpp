GF_Box *enca_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MPEGAudioSampleEntryBox, GF_ISOM_BOX_TYPE_ENCA);
	gf_isom_audio_sample_entry_init((GF_AudioSampleEntryBox*)tmp);
	return (GF_Box *)tmp;
}