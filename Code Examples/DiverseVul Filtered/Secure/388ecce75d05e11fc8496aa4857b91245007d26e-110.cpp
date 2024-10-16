void audio_sample_entry_box_del(GF_Box *s)
{
	GF_MPEGAudioSampleEntryBox *ptr = (GF_MPEGAudioSampleEntryBox *)s;
	if (ptr == NULL) return;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)s);
	if (ptr->slc) gf_odf_desc_del((GF_Descriptor *)ptr->slc);
	gf_free(ptr);
}