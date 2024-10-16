extern int lxc_rmdir_onedev(char *path, const char *exclude)
{
	struct stat mystat;
	bool onedev = true;

	if (is_native_overlayfs(path)) {
		onedev = false;
	}

	if (lstat(path, &mystat) < 0) {
		if (errno == ENOENT)
			return 0;
		ERROR("Failed to stat %s", path);
		return -1;
	}

	return _recursive_rmdir(path, mystat.st_dev, exclude, 0, onedev);
}