static int sqfs_tokenize(char **tokens, int count, const char *str)
{
	int i, j, ret = 0;
	char *aux, *strc;

	strc = strdup(str);
	if (!strc)
		return -ENOMEM;

	if (!strcmp(strc, "/")) {
		tokens[0] = strdup(strc);
		if (!tokens[0]) {
			ret = -ENOMEM;
			goto free_strc;
		}
	} else {
		for (j = 0; j < count; j++) {
			aux = strtok(!j ? strc : NULL, "/");
			tokens[j] = strdup(aux);
			if (!tokens[j]) {
				for (i = 0; i < j; i++)
					free(tokens[i]);
				ret = -ENOMEM;
				goto free_strc;
			}
		}
	}

free_strc:
	free(strc);

	return ret;
}