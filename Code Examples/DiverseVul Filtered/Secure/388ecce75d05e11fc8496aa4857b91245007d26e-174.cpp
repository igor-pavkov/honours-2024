void mvhd_box_del(GF_Box *s)
{
	GF_MovieHeaderBox *ptr = (GF_MovieHeaderBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}