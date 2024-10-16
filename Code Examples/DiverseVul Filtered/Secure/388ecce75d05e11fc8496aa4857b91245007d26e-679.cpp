GF_Err elng_box_read(GF_Box *s, GF_BitStream *bs)
{
	GF_ExtendedLanguageBox *ptr = (GF_ExtendedLanguageBox *)s;

	if (ptr->size) {
		ptr->extended_language = (char*)gf_malloc((u32) ptr->size);
		if (ptr->extended_language == NULL) return GF_OUT_OF_MEM;
		gf_bs_read_data(bs, ptr->extended_language, (u32) ptr->size);
		/*safety check in case the string is not null-terminated*/
		if (ptr->extended_language[ptr->size-1]) {
			char *str = (char*)gf_malloc((u32) ptr->size + 1);
			if (!str) return GF_OUT_OF_MEM;
			memcpy(str, ptr->extended_language, (u32) ptr->size);
			str[ptr->size] = 0;
			gf_free(ptr->extended_language);
			ptr->extended_language = str;
		}
	}
	return GF_OK;
}