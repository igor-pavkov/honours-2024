static char **get_groupnames(void)
{
	int ngroups;
	gid_t *group_ids;
	int ret, i;
	char **groupnames;
	struct group grent;
	struct group *grentp = NULL;
	char *buf;
	size_t bufsize;

	ngroups = getgroups(0, NULL);
	if (ngroups < 0) {
		usernic_error("Failed to get number of groups the user "
			      "belongs to: %s\n", strerror(errno));
		return NULL;
	}
	if (ngroups == 0)
		return NULL;

	group_ids = malloc(sizeof(gid_t) * ngroups);
	if (!group_ids) {
		usernic_error("Failed to allocate memory while getting groups "
			      "the user belongs to: %s\n",
			      strerror(errno));
		return NULL;
	}

	ret = getgroups(ngroups, group_ids);
	if (ret < 0) {
		free(group_ids);
		usernic_error("Failed to get process groups: %s\n",
			      strerror(errno));
		return NULL;
	}

	groupnames = malloc(sizeof(char *) * (ngroups + 1));
	if (!groupnames) {
		free(group_ids);
		usernic_error("Failed to allocate memory while getting group "
			      "names: %s\n",
			      strerror(errno));
		return NULL;
	}

	memset(groupnames, 0, sizeof(char *) * (ngroups + 1));

	bufsize = sysconf(_SC_GETGR_R_SIZE_MAX);
	if (bufsize == -1)
		bufsize = 1024;

	buf = malloc(bufsize);
	if (!buf) {
		free(group_ids);
		free_groupnames(groupnames);
		usernic_error("Failed to allocate memory while getting group "
			      "names: %s\n",
			      strerror(errno));
		return NULL;
	}

	for (i = 0; i < ngroups; i++) {
		ret = getgrgid_r(group_ids[i], &grent, buf, bufsize, &grentp);
		if (!grentp) {
			if (ret == 0)
				usernic_error("%s", "Could not find matched group record\n");

			usernic_error("Failed to get group name: %s(%u)\n",
			      strerror(errno), group_ids[i]);
			free(buf);
			free(group_ids);
			free_groupnames(groupnames);
			return NULL;
		}

		groupnames[i] = strdup(grent.gr_name);
		if (!groupnames[i]) {
			usernic_error("Failed to copy group name \"%s\"",
				      grent.gr_name);
			free(buf);
			free(group_ids);
			free_groupnames(groupnames);
			return NULL;
		}
	}

	free(buf);
	free(group_ids);

	return groupnames;
}