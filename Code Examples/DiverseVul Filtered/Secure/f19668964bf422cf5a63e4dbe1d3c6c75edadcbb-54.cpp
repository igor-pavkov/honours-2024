void frma_box_del(GF_Box *s)
{
	GF_OriginalFormatBox *ptr = (GF_OriginalFormatBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}