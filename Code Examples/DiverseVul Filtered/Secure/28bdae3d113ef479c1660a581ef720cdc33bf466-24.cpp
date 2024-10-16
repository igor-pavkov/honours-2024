static int open_fuse_device(char **devp)
{
	int fd = try_open_fuse_device(devp);
	if (fd >= -1)
		return fd;

	fprintf(stderr,
		"%s: fuse device not found, try 'modprobe fuse' first\n",
		progname);

	return -1;
}