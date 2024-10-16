void iSFM_box_del(GF_Box *s)
{
	GF_ISMASampleFormatBox *ptr = (GF_ISMASampleFormatBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}