static int may_unmount(const char *mnt, int quiet)
{
	struct mntent *entp;
	FILE *fp;
	const char *user = NULL;
	char uidstr[32];
	unsigned uidlen = 0;
	int found;
	const char *mtab = _PATH_MOUNTED;

	user = get_user_name();
	if (user == NULL)
		return -1;

	fp = setmntent(mtab, "r");
	if (fp == NULL) {
		fprintf(stderr, "%s: failed to open %s: %s\n", progname, mtab,
			strerror(errno));
		return -1;
	}

	uidlen = sprintf(uidstr, "%u", getuid());

	found = 0;
	while ((entp = getmntent(fp)) != NULL) {
		if (!found && strcmp(entp->mnt_dir, mnt) == 0 &&
		    (strcmp(entp->mnt_type, "fuse") == 0 ||
		     strcmp(entp->mnt_type, "fuseblk") == 0 ||
		     strncmp(entp->mnt_type, "fuse.", 5) == 0 ||
		     strncmp(entp->mnt_type, "fuseblk.", 8) == 0)) {
			char *p = strstr(entp->mnt_opts, "user=");
			if (p &&
			    (p == entp->mnt_opts || *(p-1) == ',') &&
			    strcmp(p + 5, user) == 0) {
				found = 1;
				break;
			}
			/* /etc/mtab is a link pointing to
			   /proc/mounts: */
			else if ((p =
				  strstr(entp->mnt_opts, "user_id=")) &&
				 (p == entp->mnt_opts ||
				  *(p-1) == ',') &&
				 strncmp(p + 8, uidstr, uidlen) == 0 &&
				 (*(p+8+uidlen) == ',' ||
				  *(p+8+uidlen) == '\0')) {
				found = 1;
				break;
			}
		}
	}
	endmntent(fp);

	if (!found) {
		if (!quiet)
			fprintf(stderr,
				"%s: entry for %s not found in %s\n",
				progname, mnt, mtab);
		return -1;
	}

	return 0;
}