int sys_fallocate(int fd, enum vfs_fallocate_mode mode, off_t offset, off_t len)
{
#if defined(HAVE_LINUX_FALLOCATE64) || defined(HAVE_LINUX_FALLOCATE)
	int lmode;
	switch (mode) {
	case VFS_FALLOCATE_EXTEND_SIZE:
		lmode = 0;
		break;
	case VFS_FALLOCATE_KEEP_SIZE:
		lmode = FALLOC_FL_KEEP_SIZE;
		break;
	default:
		errno = EINVAL;
		return -1;
	}
#if defined(HAVE_LINUX_FALLOCATE)
	return fallocate(fd, lmode, offset, len);
#endif
#else
	/* TODO - plumb in fallocate from other filesysetms like VXFS etc. JRA. */
	errno = ENOSYS;
	return -1;
#endif
}