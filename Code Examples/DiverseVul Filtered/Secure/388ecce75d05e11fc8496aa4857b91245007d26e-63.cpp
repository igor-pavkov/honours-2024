void kind_box_del(GF_Box *s)
{
	GF_KindBox *ptr = (GF_KindBox *) s;
	if (ptr == NULL) return;
	if (ptr->schemeURI) gf_free(ptr->schemeURI);
	if (ptr->value) gf_free(ptr->value);
	gf_free(ptr);
}