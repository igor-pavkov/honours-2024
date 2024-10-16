void trgt_box_del(GF_Box *s)
{
	GF_TrackGroupTypeBox *ptr = (GF_TrackGroupTypeBox *)s;
	if (ptr == NULL) return;
	gf_free(ptr);
}