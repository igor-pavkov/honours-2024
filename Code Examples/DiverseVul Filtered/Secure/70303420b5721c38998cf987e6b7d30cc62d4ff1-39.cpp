static int regex_match_front(char *str, struct regex *r, int len)
{
	if (len && len < r->len)
		return 0;

	return strncmp(str, r->pattern, r->len) == 0;
}