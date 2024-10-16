void moov_box_del(GF_Box *s)
{
	GF_MovieBox *ptr = (GF_MovieBox *)s;
	if (ptr == NULL) return;
	gf_list_del(ptr->trackList);
	gf_free(ptr);
}