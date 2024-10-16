char *lxc_append_paths(const char *first, const char *second)
{
	int ret;
	size_t len;
	char *result = NULL;
	const char *pattern = "%s%s";

	len = strlen(first) + strlen(second) + 1;
	if (second[0] != '/') {
		len += 1;
		pattern = "%s/%s";
	}

	result = calloc(1, len);
	if (!result)
		return NULL;

	ret = snprintf(result, len, pattern, first, second);
	if (ret < 0 || (size_t)ret >= len) {
		free(result);
		return NULL;
	}

	return result;
}