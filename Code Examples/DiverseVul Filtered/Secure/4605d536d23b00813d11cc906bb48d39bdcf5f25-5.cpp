static char *substring_conf(char *start, int len, char quote)
{
	char *result = emalloc(len + 1);
	char *resp = result;
	int i;

	for (i = 0; i < len && start[i] != quote; ++i) {
		if (start[i] == '\\' && (start[i + 1] == '\\' || (quote && start[i + 1] == quote))) {
			*resp++ = start[++i];
		} else {
			*resp++ = start[i];
		}
	}

	*resp = '\0';
	return result;
}