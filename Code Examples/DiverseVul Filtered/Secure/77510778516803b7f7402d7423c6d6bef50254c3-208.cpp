void lsrc_box_del(GF_Box *s)
{
	GF_LASERConfigurationBox *ptr = (GF_LASERConfigurationBox *)s;
	if (ptr == NULL) return;
	if (ptr->hdr) gf_free(ptr->hdr);
	gf_free(ptr);
}