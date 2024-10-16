int sys_posix_fallocate(int fd, off_t offset, off_t len)
{
#if defined(HAVE_POSIX_FALLOCATE) && !defined(HAVE_BROKEN_POSIX_FALLOCATE)
	return posix_fallocate(fd, offset, len);
#elif defined(F_RESVSP64)
	/* this handles XFS on IRIX */
	struct flock64 fl;
	off_t new_len = offset + len;
	int ret;
	struct stat64 sbuf;

	/* unlikely to get a too large file on a 64bit system but ... */
	if (new_len < 0)
		return EFBIG;

	fl.l_whence = SEEK_SET;
	fl.l_start = offset;
	fl.l_len = len;

	ret=fcntl(fd, F_RESVSP64, &fl);

	if (ret != 0)
		return errno;

	/* Make sure the file gets enlarged after we allocated space: */
	fstat64(fd, &sbuf);
	if (new_len > sbuf.st_size)
		ftruncate64(fd, new_len);
	return 0;
#else
	return ENOSYS;
#endif
}