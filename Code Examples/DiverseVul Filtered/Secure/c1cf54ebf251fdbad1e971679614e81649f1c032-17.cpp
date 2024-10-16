int lxc_read_from_file(const char *filename, void *buf, size_t count)
{
	int fd = -1, saved_errno;
	ssize_t ret;

	fd = open(filename, O_RDONLY | O_CLOEXEC);
	if (fd < 0)
		return -1;

	if (!buf || !count) {
		char buf2[100];
		size_t count2 = 0;

		while ((ret = lxc_read_nointr(fd, buf2, 100)) > 0)
			count2 += ret;

		if (ret >= 0)
			ret = count2;
	} else {
		memset(buf, 0, count);
		ret = lxc_read_nointr(fd, buf, count);
	}

	if (ret < 0)
		SYSERROR("Read %s", filename);

	saved_errno = errno;
	close(fd);
	errno = saved_errno;
	return ret;
}