static int vfswrap_chdir(vfs_handle_struct *handle, const char *path)
{
	int result;

	START_PROFILE(syscall_chdir);
	result = chdir(path);
	END_PROFILE(syscall_chdir);
	return result;
}