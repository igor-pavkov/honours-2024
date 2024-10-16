void gen_sample_entry_box_del(GF_Box *s)
{
	GF_SampleEntryBox *ptr = (GF_SampleEntryBox *)s;
	if (ptr == NULL) return;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)s);
	gf_free(ptr);
}