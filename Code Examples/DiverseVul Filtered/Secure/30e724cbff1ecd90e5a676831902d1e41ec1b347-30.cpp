static int vfswrap_fchown(vfs_handle_struct *handle, files_struct *fsp, uid_t uid, gid_t gid)
{
#ifdef HAVE_FCHOWN
	int result;

	START_PROFILE(syscall_fchown);
	result = fchown(fsp->fh->fd, uid, gid);
	END_PROFILE(syscall_fchown);
	return result;
#else
	errno = ENOSYS;
	return -1;
#endif
}