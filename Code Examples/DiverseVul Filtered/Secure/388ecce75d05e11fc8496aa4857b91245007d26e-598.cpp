void stsz_box_del(GF_Box *s)
{
	GF_SampleSizeBox *ptr = (GF_SampleSizeBox *)s;
	if (ptr == NULL) return;
	if (ptr->sizes) gf_free(ptr->sizes);
	gf_free(ptr);
}