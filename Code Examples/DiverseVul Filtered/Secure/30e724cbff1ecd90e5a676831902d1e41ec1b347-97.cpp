static char *vfswrap_realpath(vfs_handle_struct *handle, const char *path)
{
	char *result;

	START_PROFILE(syscall_realpath);
#ifdef REALPATH_TAKES_NULL
	result = realpath(path, NULL);
#else
	result = SMB_MALLOC_ARRAY(char, PATH_MAX+1);
	if (result) {
		char *resolved_path = realpath(path, result);
		if (!resolved_path) {
			SAFE_FREE(result);
		} else {
			/* SMB_ASSERT(result == resolved_path) ? */
			result = resolved_path;
		}
	}
#endif
	END_PROFILE(syscall_realpath);
	return result;
}