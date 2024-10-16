static DIR *vfswrap_opendir(vfs_handle_struct *handle, const char *fname, const char *mask, uint32 attr)
{
	DIR *result;

	START_PROFILE(syscall_opendir);
	result = opendir(fname);
	END_PROFILE(syscall_opendir);
	return result;
}