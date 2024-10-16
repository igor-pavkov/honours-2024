static long vfswrap_telldir(vfs_handle_struct *handle, DIR *dirp)
{
	long result;
	START_PROFILE(syscall_telldir);
	result = telldir(dirp);
	END_PROFILE(syscall_telldir);
	return result;
}