int longest_ancestor_length(const char *path, struct string_list *prefixes)
{
	int i, max_len = -1;

	if (!strcmp(path, "/"))
		return -1;

	for (i = 0; i < prefixes->nr; i++) {
		const char *ceil = prefixes->items[i].string;
		int len = strlen(ceil);

		if (len == 1 && ceil[0] == '/')
			len = 0; /* root matches anything, with length 0 */
		else if (!strncmp(path, ceil, len) && path[len] == '/')
			; /* match of length len */
		else
			continue; /* no match */

		if (len > max_len)
			max_len = len;
	}

	return max_len;
}