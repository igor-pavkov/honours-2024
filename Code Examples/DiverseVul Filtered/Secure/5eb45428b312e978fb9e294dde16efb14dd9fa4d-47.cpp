int lxc_mount_proc_if_needed(const char *rootfs)
{
	char path[MAXPATHLEN];
	int link_to_pid, linklen, mypid, ret;
	char link[LXC_NUMSTRLEN64] = {0};

	ret = snprintf(path, MAXPATHLEN, "%s/proc/self", rootfs);
	if (ret < 0 || ret >= MAXPATHLEN) {
		SYSERROR("proc path name too long");
		return -1;
	}

	linklen = readlink(path, link, LXC_NUMSTRLEN64);

	ret = snprintf(path, MAXPATHLEN, "%s/proc", rootfs);
	if (ret < 0 || ret >= MAXPATHLEN) {
		SYSERROR("proc path name too long");
		return -1;
	}

	/* /proc not mounted */
	if (linklen < 0) {
		if (mkdir(path, 0755) && errno != EEXIST)
			return -1;
		goto domount;
	} else if (linklen >= LXC_NUMSTRLEN64) {
		link[linklen - 1] = '\0';
		ERROR("readlink returned truncated content: \"%s\"", link);
		return -1;
	}

	mypid = lxc_raw_getpid();
	INFO("I am %d, /proc/self points to \"%s\"", mypid, link);

	if (lxc_safe_int(link, &link_to_pid) < 0)
		return -1;

	/* correct procfs is already mounted */
	if (link_to_pid == mypid)
		return 0;

	ret = umount2(path, MNT_DETACH);
	if (ret < 0)
		WARN("failed to umount \"%s\" with MNT_DETACH", path);

domount:
	/* rootfs is NULL */
	if (!strcmp(rootfs, ""))
		ret = mount("proc", path, "proc", 0, NULL);
	else
		ret = safe_mount("proc", path, "proc", 0, NULL, rootfs);
	if (ret < 0)
		return -1;

	INFO("mounted /proc in container for security transition");
	return 1;
}