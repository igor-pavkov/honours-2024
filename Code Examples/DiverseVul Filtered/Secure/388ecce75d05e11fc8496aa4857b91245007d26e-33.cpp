GF_Err elng_box_write(GF_Box *s, GF_BitStream *bs)
{
	GF_Err e;
	GF_ExtendedLanguageBox *ptr = (GF_ExtendedLanguageBox *)s;
	e = gf_isom_full_box_write(s, bs);
	if (e) return e;
	if (ptr->extended_language) {
		gf_bs_write_data(bs, ptr->extended_language, (u32)(strlen(ptr->extended_language)+1));
	}
	return GF_OK;
}