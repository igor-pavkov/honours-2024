ssize_t mingw_write(int fd, const void *buf, size_t len)
{
	ssize_t result = write(fd, buf, len);

	if (result < 0 && errno == EINVAL && buf) {
		/* check if fd is a pipe */
		HANDLE h = (HANDLE) _get_osfhandle(fd);
		if (GetFileType(h) == FILE_TYPE_PIPE)
			errno = EPIPE;
		else
			errno = EINVAL;
	}

	return result;
}