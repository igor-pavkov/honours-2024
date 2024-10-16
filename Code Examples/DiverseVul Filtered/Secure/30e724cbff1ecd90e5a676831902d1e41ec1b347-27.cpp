static int vfswrap_chmod_acl(vfs_handle_struct *handle, const char *name, mode_t mode)
{
#ifdef HAVE_NO_ACL
	errno = ENOSYS;
	return -1;
#else
	int result;

	START_PROFILE(chmod_acl);
	result = chmod_acl(handle->conn, name, mode);
	END_PROFILE(chmod_acl);
	return result;
#endif
}