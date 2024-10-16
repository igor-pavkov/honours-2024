GF_Box *audio_sample_entry_box_new()
{
	ISOM_DECL_BOX_ALLOC(GF_MPEGAudioSampleEntryBox, GF_ISOM_BOX_TYPE_MP4A);
	gf_isom_audio_sample_entry_init((GF_AudioSampleEntryBox*)tmp);
	return (GF_Box *)tmp;
}