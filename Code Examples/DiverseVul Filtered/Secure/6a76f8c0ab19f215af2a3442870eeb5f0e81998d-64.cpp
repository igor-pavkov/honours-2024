static int ftrace_match(char *str, char *regex, int len, int type)
{
	int matched = 0;
	int slen;

	switch (type) {
	case MATCH_FULL:
		if (strcmp(str, regex) == 0)
			matched = 1;
		break;
	case MATCH_FRONT_ONLY:
		if (strncmp(str, regex, len) == 0)
			matched = 1;
		break;
	case MATCH_MIDDLE_ONLY:
		if (strstr(str, regex))
			matched = 1;
		break;
	case MATCH_END_ONLY:
		slen = strlen(str);
		if (slen >= len && memcmp(str + slen - len, regex, len) == 0)
			matched = 1;
		break;
	}

	return matched;
}