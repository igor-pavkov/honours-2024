void stsd_box_del(GF_Box *s)
{
	GF_SampleDescriptionBox *ptr = (GF_SampleDescriptionBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}