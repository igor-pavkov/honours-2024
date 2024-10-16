int set_nonblock(int fd)
{
	int flags;

	flags = fcntl(fd, F_GETFL, 0);
	if (!flags)
		(void)fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	return fd;
}