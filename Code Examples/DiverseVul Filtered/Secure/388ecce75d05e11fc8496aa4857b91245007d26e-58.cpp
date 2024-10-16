void stsc_box_del(GF_Box *s)
{
	GF_SampleToChunkBox *ptr = (GF_SampleToChunkBox *)s;
	if (ptr == NULL) return;
	if (ptr->entries) gf_free(ptr->entries);
	gf_free(ptr);
}