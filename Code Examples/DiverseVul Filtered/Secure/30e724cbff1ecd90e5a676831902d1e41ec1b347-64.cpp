static char *vfswrap_getwd(vfs_handle_struct *handle)
{
	char *result;

	START_PROFILE(syscall_getwd);
	result = sys_getwd();
	END_PROFILE(syscall_getwd);
	return result;
}