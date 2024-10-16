static int unmount_fuse_locked(const char *mnt, int quiet, int lazy)
{
	int res;
	char *copy;
	const char *last;
	int umount_flags = lazy ? UMOUNT_DETACH : 0;

	if (getuid() != 0) {
		res = may_unmount(mnt, quiet);
		if (res == -1)
			return -1;
	}

	copy = strdup(mnt);
	if (copy == NULL) {
		fprintf(stderr, "%s: failed to allocate memory\n", progname);
		return -1;
	}

	res = chdir_to_parent(copy, &last);
	if (res == -1)
		goto out;

	if (umount_nofollow_support()) {
		umount_flags |= UMOUNT_NOFOLLOW;
	} else {
		res = check_is_mount(last, mnt);
		if (res == -1)
			goto out;
	}

	res = umount2(last, umount_flags);
	if (res == -1 && !quiet) {
		fprintf(stderr, "%s: failed to unmount %s: %s\n",
			progname, mnt, strerror(errno));
	}

out:
	if (res == -1)
		return -1;

	res = chdir("/");
	if (res == -1) {
		fprintf(stderr, "%s: failed to chdir to '/'\n", progname);
		return -1;
	}

	return fuse_mnt_remove_mount(progname, mnt);
}