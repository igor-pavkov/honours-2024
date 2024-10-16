static void vfswrap_seekdir(vfs_handle_struct *handle, DIR *dirp, long offset)
{
	START_PROFILE(syscall_seekdir);
	seekdir(dirp, offset);
	END_PROFILE(syscall_seekdir);
}