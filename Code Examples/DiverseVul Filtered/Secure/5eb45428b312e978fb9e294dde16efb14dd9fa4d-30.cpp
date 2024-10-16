int lxc_make_tmpfile(char *template, bool rm)
{
	int fd, ret;

	fd = mkstemp(template);
	if (fd < 0)
		return -1;

	if (!rm)
		return fd;

	ret = unlink(template);
	if (ret < 0) {
		close(fd);
		return -1;
	}

	return fd;
}