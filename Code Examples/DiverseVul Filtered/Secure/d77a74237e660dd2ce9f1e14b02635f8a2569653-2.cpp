ssize_t sys_pread(int fd, void *buf, size_t count, off_t off)
{
	ssize_t ret;

	do {
		ret = pread(fd, buf, count, off);
	} while (ret == -1 && errno == EINTR);
	return ret;
}