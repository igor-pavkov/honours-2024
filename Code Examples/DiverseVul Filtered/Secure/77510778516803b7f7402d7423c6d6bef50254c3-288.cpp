void svhd_box_del(GF_Box *s)
{
	GF_SphericalVideoInfoBox *ptr = (GF_SphericalVideoInfoBox *)s;
	if (ptr->string) gf_free(ptr->string);
	gf_free(s);
}