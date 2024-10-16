static int vfswrap_chflags(vfs_handle_struct *handle, const char *path,
			   unsigned int flags)
{
#ifdef HAVE_CHFLAGS
	return chflags(path, flags);
#else
	errno = ENOSYS;
	return -1;
#endif
}