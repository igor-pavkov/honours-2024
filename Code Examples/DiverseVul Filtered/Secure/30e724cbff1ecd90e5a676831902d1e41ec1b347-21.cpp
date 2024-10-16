static int vfswrap_chown(vfs_handle_struct *handle, const char *path, uid_t uid, gid_t gid)
{
	int result;

	START_PROFILE(syscall_chown);
	result = chown(path, uid, gid);
	END_PROFILE(syscall_chown);
	return result;
}