void cslg_box_del(GF_Box *s)
{
	GF_CompositionToDecodeBox *ptr = (GF_CompositionToDecodeBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
	return;
}