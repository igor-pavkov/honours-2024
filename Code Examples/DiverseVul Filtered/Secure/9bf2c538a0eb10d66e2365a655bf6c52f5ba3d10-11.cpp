int netns_identify_pid(const char *pidstr, char *name, int len)
{
	char net_path[PATH_MAX];
	int netns;
	struct stat netst;
	DIR *dir;
	struct dirent *entry;

	name[0] = '\0';

	snprintf(net_path, sizeof(net_path), "/proc/%s/ns/net", pidstr);
	netns = open(net_path, O_RDONLY);
	if (netns < 0) {
		fprintf(stderr, "Cannot open network namespace: %s\n",
			strerror(errno));
		return -1;
	}
	if (fstat(netns, &netst) < 0) {
		fprintf(stderr, "Stat of netns failed: %s\n",
			strerror(errno));
		return -1;
	}
	dir = opendir(NETNS_RUN_DIR);
	if (!dir) {
		/* Succeed treat a missing directory as an empty directory */
		if (errno == ENOENT)
			return 0;

		fprintf(stderr, "Failed to open directory %s:%s\n",
			NETNS_RUN_DIR, strerror(errno));
		return -1;
	}

	while ((entry = readdir(dir))) {
		char name_path[PATH_MAX];
		struct stat st;

		if (strcmp(entry->d_name, ".") == 0)
			continue;
		if (strcmp(entry->d_name, "..") == 0)
			continue;

		snprintf(name_path, sizeof(name_path), "%s/%s",	NETNS_RUN_DIR,
			entry->d_name);

		if (stat(name_path, &st) != 0)
			continue;

		if ((st.st_dev == netst.st_dev) &&
		    (st.st_ino == netst.st_ino)) {
			strlcpy(name, entry->d_name, len);
		}
	}
	closedir(dir);
	return 0;

}