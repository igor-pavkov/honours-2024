static int check_is_mount_child(void *p)
{
	const char **a = p;
	const char *last = a[0];
	const char *mnt = a[1];
	int res;
	const char *procmounts = "/proc/mounts";
	int found;
	FILE *fp;
	struct mntent *entp;
	int count;

	res = mount("", "/", "", MS_PRIVATE | MS_REC, NULL);
	if (res == -1) {
		fprintf(stderr, "%s: failed to mark mounts private: %s\n",
			progname, strerror(errno));
		return 1;
	}

	fp = setmntent(procmounts, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: failed to open %s: %s\n", progname,
			procmounts, strerror(errno));
		return 1;
	}

	count = 0;
	while (getmntent(fp) != NULL)
		count++;
	endmntent(fp);

	fp = setmntent(procmounts, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: failed to open %s: %s\n", progname,
			procmounts, strerror(errno));
		return 1;
	}

	res = mount(".", "/", "", MS_BIND | MS_REC, NULL);
	if (res == -1) {
		fprintf(stderr, "%s: failed to bind parent to /: %s\n",
			progname, strerror(errno));
		return 1;
	}

	found = 0;
	while ((entp = getmntent(fp)) != NULL) {
		if (count > 0) {
			count--;
			continue;
		}
		if (entp->mnt_dir[0] == '/' &&
		    strcmp(entp->mnt_dir + 1, last) == 0) {
			found = 1;
			break;
		}
	}
	endmntent(fp);

	if (!found) {
		fprintf(stderr, "%s: %s not mounted\n", progname, mnt);
		return 1;
	}

	return 0;
}