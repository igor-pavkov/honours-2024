void tmcd_box_del(GF_Box *s)
{
	if (s == NULL) return;
	gf_isom_sample_entry_predestroy((GF_SampleEntryBox *)s);
	gf_free(s);
}