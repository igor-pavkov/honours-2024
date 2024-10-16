void mdat_box_del(GF_Box *s)
{
	GF_MediaDataBox *ptr = (GF_MediaDataBox *)s;
	if (!s) return;

	if (ptr->data) gf_free(ptr->data);
	gf_free(ptr);
}