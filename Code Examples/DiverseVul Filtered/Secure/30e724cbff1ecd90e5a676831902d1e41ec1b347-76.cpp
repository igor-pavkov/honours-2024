static int vfswrap_lchown(vfs_handle_struct *handle, const char *path, uid_t uid, gid_t gid)
{
	int result;

	START_PROFILE(syscall_lchown);
	result = lchown(path, uid, gid);
	END_PROFILE(syscall_lchown);
	return result;
}