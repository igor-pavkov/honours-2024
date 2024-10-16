int open_devnull(void)
{
	int fd = open("/dev/null", O_RDWR);

	if (fd < 0)
		SYSERROR("Can't open /dev/null");

	return fd;
}