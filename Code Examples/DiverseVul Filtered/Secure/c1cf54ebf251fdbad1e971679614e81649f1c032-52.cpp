bool lxc_string_in_list(const char *needle, const char *haystack, char _sep)
{
	char *token, *str;
	char sep[2] = { _sep, '\0' };
	size_t len;

	if (!haystack || !needle)
		return 0;

	len = strlen(haystack);
	str = alloca(len + 1);
	(void)strlcpy(str, haystack, len + 1);

	lxc_iterate_parts(token, str, sep)
		if (strcmp(needle, token) == 0)
			return 1;

	return 0;
}