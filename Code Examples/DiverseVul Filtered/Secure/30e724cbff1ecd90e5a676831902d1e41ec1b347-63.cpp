static DIR *vfswrap_fdopendir(vfs_handle_struct *handle,
			files_struct *fsp,
			const char *mask,
			uint32 attr)
{
	DIR *result;

	START_PROFILE(syscall_fdopendir);
	result = sys_fdopendir(fsp->fh->fd);
	END_PROFILE(syscall_fdopendir);
	return result;
}