static bool vfswrap_getlock(vfs_handle_struct *handle, files_struct *fsp, off_t *poffset, off_t *pcount, int *ptype, pid_t *ppid)
{
	bool result;

	START_PROFILE(syscall_fcntl_getlock);
	result =  fcntl_getlock(fsp->fh->fd, poffset, pcount, ptype, ppid);
	END_PROFILE(syscall_fcntl_getlock);
	return result;
}