static int mount_notrunc(const char *source, const char *target,
			 const char *filesystemtype, unsigned long mountflags,
			 const char *data) {
	if (strlen(data) > sysconf(_SC_PAGESIZE) - 1) {
		fprintf(stderr, "%s: mount options too long\n", progname);
		errno = EINVAL;
		return -1;
	}
	return mount(source, target, filesystemtype, mountflags, data);
}