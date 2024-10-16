char *on_path(const char *cmd, const char *rootfs) {
	char *path = NULL;
	char *entry = NULL;
	char *saveptr = NULL;
	char cmdpath[MAXPATHLEN];
	int ret;

	path = getenv("PATH");
	if (!path)
		return NULL;

	path = strdup(path);
	if (!path)
		return NULL;

	entry = strtok_r(path, ":", &saveptr);
	while (entry) {
		if (rootfs)
			ret = snprintf(cmdpath, MAXPATHLEN, "%s/%s/%s", rootfs, entry, cmd);
		else
			ret = snprintf(cmdpath, MAXPATHLEN, "%s/%s", entry, cmd);

		if (ret < 0 || ret >= MAXPATHLEN)
			goto next_loop;

		if (access(cmdpath, X_OK) == 0) {
			free(path);
			return strdup(cmdpath);
		}

next_loop:
		entry = strtok_r(NULL, ":", &saveptr);
	}

	free(path);
	return NULL;
}