static bool vfswrap_lock(vfs_handle_struct *handle, files_struct *fsp, int op, off_t offset, off_t count, int type)
{
	bool result;

	START_PROFILE(syscall_fcntl_lock);
	result =  fcntl_lock(fsp->fh->fd, op, offset, count, type);
	END_PROFILE(syscall_fcntl_lock);
	return result;
}