int recursive_destroy(char *dirname)
{
	int ret;
	struct dirent *direntp;
	DIR *dir;
	int r = 0;

	dir = opendir(dirname);
	if (!dir)
		return -1;

	while ((direntp = readdir(dir))) {
		char *pathname;
		struct stat mystat;

		if (!strcmp(direntp->d_name, ".") ||
		    !strcmp(direntp->d_name, ".."))
			continue;

		pathname = must_make_path(dirname, direntp->d_name, NULL);

		ret = lstat(pathname, &mystat);
		if (ret < 0) {
			if (!r)
				WARN("Failed to stat \"%s\"", pathname);

			r = -1;
			goto next;
		}

		if (!S_ISDIR(mystat.st_mode))
			goto next;

		ret = recursive_destroy(pathname);
		if (ret < 0)
			r = -1;

	next:
		free(pathname);
	}

	ret = rmdir(dirname);
	if (ret < 0) {
		if (!r)
			SYSWARN("Failed to delete \"%s\"", dirname);

		r = -1;
	}

	ret = closedir(dir);
	if (ret < 0) {
		if (!r)
			SYSWARN("Failed to delete \"%s\"", dirname);

		r = -1;
	}

	return r;
}