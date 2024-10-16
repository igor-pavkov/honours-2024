int parse_byte_size_string(const char *s, int64_t *converted)
{
	int ret, suffix_len;
	long long int conv;
	int64_t mltpl, overflow;
	char *end;
	char dup[LXC_NUMSTRLEN64 + 2];
	char suffix[3] = {0};

	if (!s || !strcmp(s, ""))
		return -EINVAL;

	end = stpncpy(dup, s, sizeof(dup) - 1);
	if (*end != '\0')
		return -EINVAL;

	if (isdigit(*(end - 1)))
		suffix_len = 0;
	else if (isalpha(*(end - 1)))
		suffix_len = 1;
	else
		return -EINVAL;

	if (suffix_len > 0 && (end - 2) == dup && !isdigit(*(end - 2)))
		return -EINVAL;

	if (suffix_len > 0 && isalpha(*(end - 2)))
		suffix_len++;

	if (suffix_len > 0) {
		memcpy(suffix, end - suffix_len, suffix_len);
		*(suffix + suffix_len) = '\0';
		*(end - suffix_len) = '\0';
	}
	dup[lxc_char_right_gc(dup, strlen(dup))] = '\0';

	ret = lxc_safe_long_long(dup, &conv);
	if (ret < 0)
		return -ret;

	if (suffix_len != 2) {
		*converted = conv;
		return 0;
	}

	if (strcasecmp(suffix, "KB") == 0)
		mltpl = 1024;
	else if (strcasecmp(suffix, "MB") == 0)
		mltpl = 1024 * 1024;
	else if (strcasecmp(suffix, "GB") == 0)
		mltpl = 1024 * 1024 * 1024;
	else
		return -EINVAL;

	overflow = conv * mltpl;
	if (conv != 0 && (overflow / conv) != mltpl)
		return -ERANGE;

	*converted = overflow;
	return 0;
}