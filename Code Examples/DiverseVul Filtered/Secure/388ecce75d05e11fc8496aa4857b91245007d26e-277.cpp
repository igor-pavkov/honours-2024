void stts_box_del(GF_Box *s)
{
	GF_TimeToSampleBox *ptr = (GF_TimeToSampleBox *)s;
	if (ptr->entries) gf_free(ptr->entries);
	gf_free(ptr);
}