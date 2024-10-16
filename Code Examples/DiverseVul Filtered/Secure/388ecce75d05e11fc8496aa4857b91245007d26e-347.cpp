void tfra_box_del(GF_Box *s)
{
	GF_TrackFragmentRandomAccessBox *ptr = (GF_TrackFragmentRandomAccessBox *)s;
	if (ptr == NULL) return;
	if (ptr->entries) gf_free(ptr->entries);
	gf_free(ptr);
}