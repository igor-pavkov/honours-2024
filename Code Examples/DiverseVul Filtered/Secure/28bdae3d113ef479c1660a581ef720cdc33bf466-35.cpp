static int check_perm(const char **mntp, struct stat *stbuf, int *mountpoint_fd)
{
	int res;
	const char *mnt = *mntp;
	const char *origmnt = mnt;

	res = lstat(mnt, stbuf);
	if (res == -1) {
		fprintf(stderr, "%s: failed to access mountpoint %s: %s\n",
			progname, mnt, strerror(errno));
		return -1;
	}

	/* No permission checking is done for root */
	if (getuid() == 0)
		return 0;

	if (S_ISDIR(stbuf->st_mode)) {
		res = chdir(mnt);
		if (res == -1) {
			fprintf(stderr,
				"%s: failed to chdir to mountpoint: %s\n",
				progname, strerror(errno));
			return -1;
		}
		mnt = *mntp = ".";
		res = lstat(mnt, stbuf);
		if (res == -1) {
			fprintf(stderr,
				"%s: failed to access mountpoint %s: %s\n",
				progname, origmnt, strerror(errno));
			return -1;
		}

		if ((stbuf->st_mode & S_ISVTX) && stbuf->st_uid != getuid()) {
			fprintf(stderr, "%s: mountpoint %s not owned by user\n",
				progname, origmnt);
			return -1;
		}

		res = access(mnt, W_OK);
		if (res == -1) {
			fprintf(stderr, "%s: user has no write access to mountpoint %s\n",
				progname, origmnt);
			return -1;
		}
	} else if (S_ISREG(stbuf->st_mode)) {
		static char procfile[256];
		*mountpoint_fd = open(mnt, O_WRONLY);
		if (*mountpoint_fd == -1) {
			fprintf(stderr, "%s: failed to open %s: %s\n",
				progname, mnt, strerror(errno));
			return -1;
		}
		res = fstat(*mountpoint_fd, stbuf);
		if (res == -1) {
			fprintf(stderr,
				"%s: failed to access mountpoint %s: %s\n",
				progname, mnt, strerror(errno));
			return -1;
		}
		if (!S_ISREG(stbuf->st_mode)) {
			fprintf(stderr,
				"%s: mountpoint %s is no longer a regular file\n",
				progname, mnt);
			return -1;
		}

		sprintf(procfile, "/proc/self/fd/%i", *mountpoint_fd);
		*mntp = procfile;
	} else {
		fprintf(stderr,
			"%s: mountpoint %s is not a directory or a regular file\n",
			progname, mnt);
		return -1;
	}


	return 0;
}