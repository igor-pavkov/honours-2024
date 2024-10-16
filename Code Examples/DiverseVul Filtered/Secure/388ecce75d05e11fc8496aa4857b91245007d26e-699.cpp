void hmhd_box_del(GF_Box *s)
{
	GF_HintMediaHeaderBox *ptr = (GF_HintMediaHeaderBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}