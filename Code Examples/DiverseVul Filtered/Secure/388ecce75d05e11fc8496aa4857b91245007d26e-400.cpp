void mp4s_box_del(GF_Box *s)
{
	GF_MPEGSampleEntryBox *ptr = (GF_MPEGSampleEntryBox *)s;
	if (ptr == NULL) return;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)s);
	if (ptr->slc) gf_odf_desc_del((GF_Descriptor *)ptr->slc);
	gf_free(ptr);
}