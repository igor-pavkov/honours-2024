void video_sample_entry_box_del(GF_Box *s)
{
	GF_MPEGVisualSampleEntryBox *ptr = (GF_MPEGVisualSampleEntryBox *)s;
	if (ptr == NULL) return;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)s);

	if (ptr->slc) gf_odf_desc_del((GF_Descriptor *)ptr->slc);
	/*for publishing*/
	if (ptr->emul_esd) gf_odf_desc_del((GF_Descriptor *)ptr->emul_esd);
	gf_free(ptr);
}