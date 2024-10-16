static int vfswrap_readlink(vfs_handle_struct *handle, const char *path, char *buf, size_t bufsiz)
{
	int result;

	START_PROFILE(syscall_readlink);
	result = readlink(path, buf, bufsiz);
	END_PROFILE(syscall_readlink);
	return result;
}