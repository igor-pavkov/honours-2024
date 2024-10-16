static int regex_match_full(char *str, struct regex *r, int len)
{
	/* len of zero means str is dynamic and ends with '\0' */
	if (!len)
		return strcmp(str, r->pattern) == 0;

	return strncmp(str, r->pattern, len) == 0;
}