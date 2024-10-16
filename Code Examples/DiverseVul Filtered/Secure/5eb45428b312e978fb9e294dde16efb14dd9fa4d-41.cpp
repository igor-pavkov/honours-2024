int lxc_safe_long_long(const char *numstr, long long int *converted)
{
	char *err = NULL;
	signed long long int sli;

	errno = 0;
	sli = strtoll(numstr, &err, 0);
	if (errno == ERANGE && (sli == LLONG_MAX || sli == LLONG_MIN))
		return -ERANGE;

	if (errno != 0 && sli == 0)
		return -EINVAL;

	if (err == numstr || *err != '\0')
		return -EINVAL;

	*converted = sli;
	return 0;
}