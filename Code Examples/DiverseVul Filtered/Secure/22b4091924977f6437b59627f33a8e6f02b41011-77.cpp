static uint32_t unix_filetype(mode_t mode)
{
	if(S_ISREG(mode))
		return UNIX_TYPE_FILE;
	else if(S_ISDIR(mode))
		return UNIX_TYPE_DIR;
#ifdef S_ISLNK
	else if(S_ISLNK(mode))
		return UNIX_TYPE_SYMLINK;
#endif
#ifdef S_ISCHR
	else if(S_ISCHR(mode))
		return UNIX_TYPE_CHARDEV;
#endif
#ifdef S_ISBLK
	else if(S_ISBLK(mode))
		return UNIX_TYPE_BLKDEV;
#endif
#ifdef S_ISFIFO
	else if(S_ISFIFO(mode))
		return UNIX_TYPE_FIFO;
#endif
#ifdef S_ISSOCK
	else if(S_ISSOCK(mode))
		return UNIX_TYPE_SOCKET;
#endif

	DEBUG(0,("unix_filetype: unknown filetype %u\n", (unsigned)mode));
	return UNIX_TYPE_UNKNOWN;
}