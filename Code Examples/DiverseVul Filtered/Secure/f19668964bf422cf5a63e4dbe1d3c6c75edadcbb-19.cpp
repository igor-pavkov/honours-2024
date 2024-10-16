void schm_box_del(GF_Box *s)
{
	GF_SchemeTypeBox *ptr = (GF_SchemeTypeBox *)s;
	if (ptr == NULL) return;
	if (ptr->URI) gf_free(ptr->URI);
	gf_free(ptr);
}