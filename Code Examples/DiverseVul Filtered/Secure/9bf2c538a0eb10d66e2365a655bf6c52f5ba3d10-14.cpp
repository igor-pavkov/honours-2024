static int netns_get_name(int nsid, char *name)
{
	struct dirent *entry;
	DIR *dir;
	int id;

	dir = opendir(NETNS_RUN_DIR);
	if (!dir)
		return -ENOENT;

	while ((entry = readdir(dir)) != NULL) {
		if (strcmp(entry->d_name, ".") == 0)
			continue;
		if (strcmp(entry->d_name, "..") == 0)
			continue;
		id = get_netnsid_from_name(entry->d_name);

		if (nsid == id) {
			strcpy(name, entry->d_name);
			closedir(dir);
			return 0;
		}
	}
	closedir(dir);
	return -ENOENT;
}