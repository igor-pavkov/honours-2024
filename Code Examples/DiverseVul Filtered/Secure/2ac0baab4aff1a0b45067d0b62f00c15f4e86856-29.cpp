static int sqfs_count_tokens(const char *filename)
{
	int token_count = 1, l;

	for (l = 1; l < strlen(filename); l++) {
		if (filename[l] == '/')
			token_count++;
	}

	/* Ignore trailing '/' in path */
	if (filename[strlen(filename) - 1] == '/')
		token_count--;

	if (!token_count)
		token_count = 1;

	return token_count;
}