void mfro_box_del(GF_Box *s)
{
	GF_MovieFragmentRandomAccessOffsetBox *ptr = (GF_MovieFragmentRandomAccessOffsetBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}