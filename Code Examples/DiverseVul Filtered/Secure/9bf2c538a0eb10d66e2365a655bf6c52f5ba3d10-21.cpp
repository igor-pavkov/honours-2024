void netns_map_init(void)
{
	static int initialized;
	struct dirent *entry;
	DIR *dir;
	int nsid;

	if (initialized || !ipnetns_have_nsid())
		return;

	dir = opendir(NETNS_RUN_DIR);
	if (!dir)
		return;

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0)
			continue;
		if (strcmp(entry->d_name, "..") == 0)
			continue;
		nsid = get_netnsid_from_name(entry->d_name);

		if (nsid >= 0)
			netns_map_add(nsid, entry->d_name);
	}
	closedir(dir);
	initialized = 1;
}