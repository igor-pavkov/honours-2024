ssize_t lxc_read_nointr_expect(int fd, void *buf, size_t count, const void *expected_buf)
{
	ssize_t ret;

	ret = lxc_read_nointr(fd, buf, count);
	if (ret <= 0)
		return ret;

	if ((size_t)ret != count)
		return -1;

	if (expected_buf && memcmp(buf, expected_buf, count) != 0) {
		errno = EINVAL;
		return -1;
	}

	return ret;
}