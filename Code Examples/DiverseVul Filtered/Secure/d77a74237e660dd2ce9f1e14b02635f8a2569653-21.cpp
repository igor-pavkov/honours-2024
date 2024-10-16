int sys_fcntl_long(int fd, int cmd, long arg)
{
	int ret;

	do {
		ret = fcntl(fd, cmd, arg);
	} while (ret == -1 && errno == EINTR);
	return ret;
}