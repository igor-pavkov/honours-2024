GF_Box *video_sample_entry_box_new()
{
	GF_MPEGVisualSampleEntryBox *tmp;
	GF_SAFEALLOC(tmp, GF_MPEGVisualSampleEntryBox);
	if (tmp == NULL) return NULL;

	gf_isom_video_sample_entry_init((GF_VisualSampleEntryBox *)tmp);
	return (GF_Box *)tmp;
}