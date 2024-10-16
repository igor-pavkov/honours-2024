bool switch_to_ns(pid_t pid, const char *ns) {
	int fd, ret;
	char nspath[MAXPATHLEN];

	/* Switch to new ns */
	ret = snprintf(nspath, MAXPATHLEN, "/proc/%d/ns/%s", pid, ns);
	if (ret < 0 || ret >= MAXPATHLEN)
		return false;

	fd = open(nspath, O_RDONLY);
	if (fd < 0) {
		SYSERROR("Failed to open %s", nspath);
		return false;
	}

	ret = setns(fd, 0);
	if (ret) {
		SYSERROR("Failed to set process %d to %s of %d.", pid, ns, fd);
		close(fd);
		return false;
	}

	close(fd);
	return true;
}