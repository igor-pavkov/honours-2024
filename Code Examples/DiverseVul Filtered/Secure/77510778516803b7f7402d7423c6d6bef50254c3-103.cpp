void trun_box_del(GF_Box *s)
{
	GF_TrackFragmentRunBox *ptr = (GF_TrackFragmentRunBox *)s;
	if (ptr == NULL) return;

	if (ptr->samples) gf_free(ptr->samples);
	if (ptr->cache) gf_bs_del(ptr->cache);
	if (ptr->sample_order) gf_free(ptr->sample_order);
	gf_free(ptr);
}