static int get_alloted(char *me, char *intype, char *link,
		       struct alloted_s **alloted)
{
	int n, ret;
	char name[100], type[100], br[100];
	char **groups;
	FILE *fin;

	int count = 0;
	size_t len = 0;
	char *line = NULL;

	fin = fopen(LXC_USERNIC_CONF, "r");
	if (!fin) {
		usernic_error("Failed to open \"%s\": %s\n", LXC_USERNIC_CONF,
			      strerror(errno));
		return -1;
	}

	groups = get_groupnames();
	while ((getline(&line, &len, fin)) != -1) {
		ret = sscanf(line, "%99[^ \t] %99[^ \t] %99[^ \t] %d", name,
			     type, br, &n);
		if (ret != 4)
			continue;

		if (strlen(name) == 0)
			continue;

		if (strcmp(name, me)) {
			if (name[0] != '@')
				continue;

			if (!name_is_in_groupnames(name + 1, groups))
				continue;
		}

		if (strcmp(type, intype))
			continue;

		if (strcmp(link, br))
			continue;

		/* Found the user or group with the appropriate settings,
		 * therefore finish the search. What to do if there are more
		 * than one applicable lines? not specified in the docs. Since
		 * getline is implemented with realloc, we don't need to free
		 * line until exiting func.
		 *
		 * If append_alloted returns NULL, e.g. due to a malloc error,
		 * we set count to 0 and break the loop, allowing cleanup and
		 * then exiting from main().
		 */
		if (!append_alloted(alloted, name, n)) {
			count = 0;
			break;
		}
		count += n;
	}

	free_groupnames(groups);
	fclose(fin);
	free(line);

	/* Now return the total number of nics that this user can create. */
	return count;
}