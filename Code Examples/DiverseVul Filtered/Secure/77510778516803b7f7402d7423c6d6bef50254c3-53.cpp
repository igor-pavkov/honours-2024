void txtc_box_del(GF_Box *s)
{
	GF_TextConfigBox *ptr = (GF_TextConfigBox*)s;
	if (ptr == NULL) return;

	if (ptr->config) gf_free(ptr->config);
	gf_free(ptr);
}