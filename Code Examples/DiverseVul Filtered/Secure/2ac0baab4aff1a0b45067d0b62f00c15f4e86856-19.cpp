static int sqfs_get_tokens_length(char **tokens, int count)
{
	int length = 0, i;

	/*
	 * 1 is added to the result of strlen to consider the slash separator
	 * between the tokens.
	 */
	for (i = 0; i < count; i++)
		length += strlen(tokens[i]) + 1;

	return length;
}