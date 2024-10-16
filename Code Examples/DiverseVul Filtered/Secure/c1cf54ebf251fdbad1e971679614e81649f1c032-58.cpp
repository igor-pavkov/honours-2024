int null_stdfds(void)
{
	int ret = -1;
	int fd;

	fd = open_devnull();
	if (fd >= 0) {
		ret = set_stdfds(fd);
		close(fd);
	}

	return ret;
}