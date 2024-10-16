void gnrm_box_del(GF_Box *s)
{
	GF_GenericSampleEntryBox *ptr = (GF_GenericSampleEntryBox *)s;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)ptr);
	if (ptr->data) gf_free(ptr->data);
	gf_free(ptr);
}