static int vfswrap_closedir(vfs_handle_struct *handle, DIR *dirp)
{
	int result;

	START_PROFILE(syscall_closedir);
	result = closedir(dirp);
	END_PROFILE(syscall_closedir);
	return result;
}