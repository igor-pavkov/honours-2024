char *must_copy_string(const char *entry)
{
	char *ret;

	if (!entry)
		return NULL;

	do {
		ret = strdup(entry);
	} while (!ret);

	return ret;
}