static int sqfs_join(char **strings, char *dest, int start, int end,
		     char separator)
{
	int i, offset = 0;

	for (i = start; i < end; i++) {
		strcpy(dest + offset, strings[i]);
		offset += strlen(strings[i]);
		if (i < end - 1)
			dest[offset++] = separator;
	}

	return offset;
}