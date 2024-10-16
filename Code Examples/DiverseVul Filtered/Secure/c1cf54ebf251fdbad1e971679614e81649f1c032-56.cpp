static char *get_nextpath(char *path, int *offsetp, int fulllen)
{
	int offset = *offsetp;

	if (offset >= fulllen)
		return NULL;

	while (path[offset] != '\0' && offset < fulllen)
		offset++;

	while (path[offset] == '\0' && offset < fulllen)
		offset++;

	*offsetp = offset;
	return (offset < fulllen) ? &path[offset] : NULL;
}