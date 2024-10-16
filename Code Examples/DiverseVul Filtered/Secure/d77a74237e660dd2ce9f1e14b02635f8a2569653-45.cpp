DIR *sys_fdopendir(int fd)
{
#if defined(HAVE_FDOPENDIR)
	return fdopendir(fd);
#else
	errno = ENOSYS;
	return NULL;
#endif
}