void PrintLanguages()
{
	u32 i=0, count = gf_lang_get_count();
	fprintf(stderr, "Supported ISO 639 languages and codes:\n\n");
	for (i=0; i<count; i++) {
		if (gf_lang_get_2cc(i)) {
			fprintf(stderr, "%s (%s - %s)\n", gf_lang_get_name(i), gf_lang_get_3cc(i), gf_lang_get_2cc(i));
		}
	}
}