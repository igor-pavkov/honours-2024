int lxc_safe_uint64(const char *numstr, uint64_t *converted, int base)
{
	char *err = NULL;
	uint64_t u;

	while (isspace(*numstr))
		numstr++;

	if (*numstr == '-')
		return -EINVAL;

	errno = 0;
	u = strtoull(numstr, &err, base);
	if (errno == ERANGE && u == ULLONG_MAX)
		return -ERANGE;

	if (err == numstr || *err != '\0')
		return -EINVAL;

	*converted = u;
	return 0;
}