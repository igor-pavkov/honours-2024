void nmhd_box_del(GF_Box *s)
{
	GF_MPEGMediaHeaderBox *ptr = (GF_MPEGMediaHeaderBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}