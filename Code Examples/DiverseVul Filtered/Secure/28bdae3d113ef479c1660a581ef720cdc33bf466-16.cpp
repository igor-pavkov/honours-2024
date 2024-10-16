static int lock_umount(void)
{
	const char *mtab_lock = _PATH_MOUNTED ".fuselock";
	int mtablock;
	int res;
	struct stat mtab_stat;

	/* /etc/mtab could be a symlink to /proc/mounts */
	if (lstat(_PATH_MOUNTED, &mtab_stat) == 0 && S_ISLNK(mtab_stat.st_mode))
		return -1;

	mtablock = open(mtab_lock, O_RDWR | O_CREAT, 0600);
	if (mtablock == -1) {
		fprintf(stderr, "%s: unable to open fuse lock file: %s\n",
			progname, strerror(errno));
		return -1;
	}
	res = lockf(mtablock, F_LOCK, 0);
	if (res < 0) {
		fprintf(stderr, "%s: error getting lock: %s\n", progname,
			strerror(errno));
		close(mtablock);
		return -1;
	}

	return mtablock;
}