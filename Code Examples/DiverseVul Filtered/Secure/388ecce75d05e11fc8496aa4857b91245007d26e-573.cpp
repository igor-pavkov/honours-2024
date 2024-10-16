void stss_box_del(GF_Box *s)
{
	GF_SyncSampleBox *ptr = (GF_SyncSampleBox *)s;
	if (ptr == NULL) return;
	if (ptr->sampleNumbers) gf_free(ptr->sampleNumbers);
	gf_free(ptr);
}