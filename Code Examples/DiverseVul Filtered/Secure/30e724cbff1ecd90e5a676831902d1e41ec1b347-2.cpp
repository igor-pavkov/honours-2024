static int vfswrap_symlink(vfs_handle_struct *handle, const char *oldpath, const char *newpath)
{
	int result;

	START_PROFILE(syscall_symlink);
	result = symlink(oldpath, newpath);
	END_PROFILE(syscall_symlink);
	return result;
}