static int regex_match_middle(char *str, struct regex *r, int len)
{
	if (!len)
		return strstr(str, r->pattern) != NULL;

	return strnstr(str, r->pattern, len) != NULL;
}