static int count_fuse_fs(void)
{
	struct mntent *entp;
	int count = 0;
	const char *mtab = _PATH_MOUNTED;
	FILE *fp = setmntent(mtab, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: failed to open %s: %s\n", progname, mtab,
			strerror(errno));
		return -1;
	}
	while ((entp = getmntent(fp)) != NULL) {
		if (strcmp(entp->mnt_type, "fuse") == 0 ||
		    strncmp(entp->mnt_type, "fuse.", 5) == 0)
			count ++;
	}
	endmntent(fp);
	return count;
}