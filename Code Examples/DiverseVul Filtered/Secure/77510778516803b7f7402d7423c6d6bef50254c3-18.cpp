void stvi_box_del(GF_Box *s)
{
	GF_StereoVideoBox *ptr = (GF_StereoVideoBox *)s;
	if (ptr == NULL) return;
	if (ptr->stereo_indication_type) gf_free(ptr->stereo_indication_type);
	gf_free(ptr);
}