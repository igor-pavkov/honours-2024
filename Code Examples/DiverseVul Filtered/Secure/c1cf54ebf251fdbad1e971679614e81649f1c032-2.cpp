char *on_path(const char *cmd, const char *rootfs) {
	char *entry = NULL, *path = NULL;
	char cmdpath[MAXPATHLEN];
	int ret;

	path = getenv("PATH");
	if (!path)
		return NULL;

	path = strdup(path);
	if (!path)
		return NULL;

	lxc_iterate_parts(entry, path, ":") {
		if (rootfs)
			ret = snprintf(cmdpath, MAXPATHLEN, "%s/%s/%s", rootfs, entry, cmd);
		else
			ret = snprintf(cmdpath, MAXPATHLEN, "%s/%s", entry, cmd);
		if (ret < 0 || ret >= MAXPATHLEN)
			continue;

		if (access(cmdpath, X_OK) == 0) {
			free(path);
			return strdup(cmdpath);
		}
	}

	free(path);
	return NULL;
}