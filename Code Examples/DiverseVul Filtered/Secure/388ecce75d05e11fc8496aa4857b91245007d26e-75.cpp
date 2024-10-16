void tkhd_box_del(GF_Box *s)
{
	GF_TrackHeaderBox *ptr = (GF_TrackHeaderBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
	return;
}