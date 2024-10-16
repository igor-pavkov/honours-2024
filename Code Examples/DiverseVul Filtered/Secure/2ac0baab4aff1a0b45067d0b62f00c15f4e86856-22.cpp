static char *sqfs_concat_tokens(char **token_list, int token_count)
{
	char *result;
	int i, length = 0, offset = 0;

	length = sqfs_get_tokens_length(token_list, token_count);

	result = malloc(length + 1);
	if (!result)
		return NULL;

	result[length] = '\0';

	for (i = 0; i < token_count; i++) {
		strcpy(result + offset, token_list[i]);
		offset += strlen(token_list[i]);
		result[offset++] = '/';
	}

	return result;
}