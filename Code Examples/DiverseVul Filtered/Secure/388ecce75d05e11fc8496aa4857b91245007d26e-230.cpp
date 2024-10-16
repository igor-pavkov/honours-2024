void elng_box_del(GF_Box *s)
{
	GF_ExtendedLanguageBox *ptr = (GF_ExtendedLanguageBox *)s;
	if (ptr == NULL) return;
	if (ptr->extended_language) gf_free(ptr->extended_language);
	gf_free(ptr);
}