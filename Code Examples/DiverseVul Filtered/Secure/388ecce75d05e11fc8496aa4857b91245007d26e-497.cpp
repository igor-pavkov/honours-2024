void mfra_box_del(GF_Box *s)
{
	GF_MovieFragmentRandomAccessBox *ptr = (GF_MovieFragmentRandomAccessBox *)s;
	if (ptr == NULL) return;
	gf_list_del(ptr->tfra_list);
	gf_free(ptr);
}