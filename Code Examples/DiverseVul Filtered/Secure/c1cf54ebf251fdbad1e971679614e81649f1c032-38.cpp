char *lxc_string_join(const char *sep, const char **parts, bool use_as_prefix)
{
	char *result;
	char **p;
	size_t sep_len = strlen(sep);
	size_t result_len = use_as_prefix * sep_len;
	size_t buf_len;

	/* calculate new string length */
	for (p = (char **)parts; *p; p++)
		result_len += (p > (char **)parts) * sep_len + strlen(*p);

	buf_len = result_len + 1;
	result = calloc(buf_len, 1);
	if (!result)
		return NULL;

	if (use_as_prefix)
		(void)strlcpy(result, sep, buf_len);

	for (p = (char **)parts; *p; p++) {
		if (p > (char **)parts)
			(void)strlcat(result, sep, buf_len);

		(void)strlcat(result, *p, buf_len);
	}

	return result;
}