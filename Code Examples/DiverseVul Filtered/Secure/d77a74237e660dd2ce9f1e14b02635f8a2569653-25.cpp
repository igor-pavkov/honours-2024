ssize_t sys_pwrite(int fd, const void *buf, size_t count, off_t off)
{
	ssize_t ret;

	do {
		ret = pwrite(fd, buf, count, off);
	} while (ret == -1 && errno == EINTR);
	return ret;
}