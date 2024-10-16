static Bool strstr_nocase(const char *text, const char *subtext, u32 subtext_len)
{
	if (!*text || !subtext || !subtext_len)
		return GF_FALSE;

	while (*text) {
		if (tolower(*text) == *subtext) {
			if (!strnicmp(text, subtext, subtext_len))
				return GF_TRUE;

		}
		text++;
	}
	return GF_FALSE;
}