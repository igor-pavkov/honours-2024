void mfhd_box_del(GF_Box *s)
{
	GF_MovieFragmentHeaderBox *ptr = (GF_MovieFragmentHeaderBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}