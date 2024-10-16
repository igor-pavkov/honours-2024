static int vfswrap_mkdir(vfs_handle_struct *handle, const char *path, mode_t mode)
{
	int result;
	bool has_dacl = False;
	char *parent = NULL;

	START_PROFILE(syscall_mkdir);

	if (lp_inherit_acls(SNUM(handle->conn))
	    && parent_dirname(talloc_tos(), path, &parent, NULL)
	    && (has_dacl = directory_has_default_acl(handle->conn, parent)))
		mode = (0777 & lp_dir_mask(SNUM(handle->conn)));

	TALLOC_FREE(parent);

	result = mkdir(path, mode);

	if (result == 0 && !has_dacl) {
		/*
		 * We need to do this as the default behavior of POSIX ACLs
		 * is to set the mask to be the requested group permission
		 * bits, not the group permission bits to be the requested
		 * group permission bits. This is not what we want, as it will
		 * mess up any inherited ACL bits that were set. JRA.
		 */
		int saved_errno = errno; /* We may get ENOSYS */
		if ((SMB_VFS_CHMOD_ACL(handle->conn, path, mode) == -1) && (errno == ENOSYS))
			errno = saved_errno;
	}

	END_PROFILE(syscall_mkdir);
	return result;
}