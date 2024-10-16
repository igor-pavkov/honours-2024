static int vfswrap_link(vfs_handle_struct *handle, const char *oldpath, const char *newpath)
{
	int result;

	START_PROFILE(syscall_link);
	result = link(oldpath, newpath);
	END_PROFILE(syscall_link);
	return result;
}