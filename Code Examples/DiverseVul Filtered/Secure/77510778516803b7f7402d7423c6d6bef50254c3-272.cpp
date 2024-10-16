void lsr1_box_del(GF_Box *s)
{
	GF_LASeRSampleEntryBox *ptr = (GF_LASeRSampleEntryBox *)s;
	if (ptr == NULL) return;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)s);
	if (ptr->slc) gf_odf_desc_del((GF_Descriptor *)ptr->slc);
	gf_free(ptr);
}