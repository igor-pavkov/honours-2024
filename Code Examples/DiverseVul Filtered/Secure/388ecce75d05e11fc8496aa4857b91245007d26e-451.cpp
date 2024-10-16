void gnrv_box_del(GF_Box *s)
{
	GF_GenericVisualSampleEntryBox *ptr = (GF_GenericVisualSampleEntryBox *)s;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)ptr);
	if (ptr->data) gf_free(ptr->data);
	gf_free(ptr);
}