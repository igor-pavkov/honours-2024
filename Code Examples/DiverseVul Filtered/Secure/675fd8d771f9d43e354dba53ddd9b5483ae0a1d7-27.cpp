static int shadow_copy2_setxattr(struct vfs_handle_struct *handle,
				 const char *fname,
				 const char *aname, const void *value,
				 size_t size, int flags)
{
	time_t timestamp;
	char *stripped;
	ssize_t ret;
	int saved_errno;
	char *conv;

	if (!shadow_copy2_strip_snapshot(talloc_tos(), handle, fname,
					 &timestamp, &stripped)) {
		return -1;
	}
	if (timestamp == 0) {
		return SMB_VFS_NEXT_SETXATTR(handle, fname, aname, value, size,
					     flags);
	}
	conv = shadow_copy2_convert(talloc_tos(), handle, stripped, timestamp);
	TALLOC_FREE(stripped);
	if (conv == NULL) {
		return -1;
	}
	ret = SMB_VFS_NEXT_SETXATTR(handle, conv, aname, value, size, flags);
	saved_errno = errno;
	TALLOC_FREE(conv);
	errno = saved_errno;
	return ret;
}