GF_Err elng_box_size(GF_Box *s)
{
	GF_ExtendedLanguageBox *ptr = (GF_ExtendedLanguageBox *)s;

	if (ptr->extended_language) {
		ptr->size += strlen(ptr->extended_language)+1;
	}
	return GF_OK;
}