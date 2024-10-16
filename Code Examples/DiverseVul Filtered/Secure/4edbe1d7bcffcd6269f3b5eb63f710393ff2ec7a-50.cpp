static int invalid_str(char *str, void *end)
{
	while ((void *) str < end)
		if (!*str++)
			return 0;

	return -EINVAL;
}